#include "goalie_number_window.h"

#include "../common/goalie_configuration.h"

#include <inttypes.h>

#define GOALIE_NUMBER_WINDOW_MIN ((int32_t)-999999)
#define GOALIE_NUMBER_WINDOW_MAX ((int32_t)999999)

// length of "-999999" (7) + 3 for "..." (truncation ellipsis, if necessary) + 1 for null terminator
#define GOALIE_NUMBER_WINDOW_VALUE_STRING_MAX_SIZE (11)

struct GoalieNumberWindow {
  Window *window;
  TextLayer *label_text_layer;
  const char *label;
  TextLayer *value_text_layer;
  char value_text[GOALIE_NUMBER_WINDOW_VALUE_STRING_MAX_SIZE];
  ActionBarLayer *action_bar_layer;
  GBitmap *up_icon;
  GBitmap *checkmark_icon;
  GBitmap *down_icon;
  GoalieNumberWindowCallbacks callbacks;
  void *callback_context;
  int32_t value;
  int32_t min;
  int32_t max;
  int32_t step_size;
};

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  GoalieNumberWindow *number_window = context;
  if (number_window && number_window->callbacks.selected) {
    number_window->callbacks.selected(number_window, number_window->callback_context);
  }
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  GoalieNumberWindow *number_window = context;
  goalie_number_window_set_value(number_window, number_window->value + number_window->step_size);
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  GoalieNumberWindow *number_window = context;
  goalie_number_window_set_value(number_window, number_window->value - number_window->step_size);
}

static void prv_click_config_provider(void *context) {
  // Using multi-click instead of single-click will allow the action bar animation to complete
  const uint8_t min_clicks = 1;
  const uint8_t max_clicks = 2;
  const uint16_t timeout_ms = 25;
  const bool last_click_only = true;
  window_multi_click_subscribe(BUTTON_ID_SELECT, min_clicks, max_clicks, timeout_ms,
                               last_click_only, prv_select_click_handler);

  const uint16_t repeat_interval_ms = 50;
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, prv_up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms,
                                          prv_down_click_handler);

  window_set_click_context(BUTTON_ID_SELECT, context);
}

static const char *prv_get_font_key_for_value(int32_t value) {
#if PBL_RECT
  if (WITHIN(value, -9999, 9999)) {
    return FONT_KEY_LECO_32_BOLD_NUMBERS;
  } else if (WITHIN(value, -99999, 99999)) {
    return FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM;
  } else {
    return FONT_KEY_LECO_20_BOLD_NUMBERS;
  }
#elif PBL_ROUND
  if (WITHIN(value, -99999, 99999)) {
    return FONT_KEY_LECO_32_BOLD_NUMBERS;
  } else {
    return FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM;
  }
#else
#error "Unknown display shape type!"
#endif
}

static void prv_update_value_text_layer(GoalieNumberWindow *number_window) {
  if (!number_window || !number_window->value_text_layer) {
    return;
  }

  const char *font_key = prv_get_font_key_for_value(number_window->value);
  text_layer_set_font(number_window->value_text_layer, fonts_get_system_font(font_key));
  text_layer_set_text(number_window->value_text_layer, number_window->value_text);
}

static void prv_window_load(Window *window) {
  GoalieNumberWindow *number_window = window_get_user_data(window);
  if (!number_window) {
    return;
  }

  Layer *window_root_layer = window_get_root_layer(window);
  const GRect window_root_layer_bounds = layer_get_bounds(window_root_layer);

  number_window->checkmark_icon = gbitmap_create_with_resource(RESOURCE_ID_CHECKMARK);
  number_window->up_icon = gbitmap_create_with_resource(RESOURCE_ID_UP);
  number_window->down_icon = gbitmap_create_with_resource(RESOURCE_ID_DOWN);

  const GTextAlignment text_layer_alignment = GTextAlignmentRight;
  const GTextOverflowMode text_layer_overflow_mode = GTextOverflowModeTrailingEllipsis;

  const GFont label_text_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  const int16_t label_text_font_height = 14;

  const int16_t value_text_max_font_height = 36;

  const int16_t horizontal_padding = 5;
  GRect text_container_frame = grect_inset(window_root_layer_bounds,
                                           GEdgeInsets(0, ACTION_BAR_WIDTH + horizontal_padding,
                                                       0, horizontal_padding));
  text_container_frame.size.h = label_text_font_height + value_text_max_font_height;
  grect_align(&text_container_frame, &window_root_layer_bounds, GAlignLeft, true /* clip */);
  text_container_frame.origin.y -= gbitmap_get_bounds(number_window->checkmark_icon).size.h / 2;

  GRect label_text_layer_frame = (GRect) {
    .size = GSize(text_container_frame.size.w, label_text_font_height),
  };
  grect_align(&label_text_layer_frame, &text_container_frame, GAlignTop, true /* clip */);
  number_window->label_text_layer = text_layer_create(label_text_layer_frame);
  TextLayer *label_text_layer = number_window->label_text_layer;
  text_layer_set_text(label_text_layer, number_window->label);
  text_layer_set_text_color(label_text_layer, GColorBlack);
  text_layer_set_background_color(label_text_layer, GColorClear);
  text_layer_set_font(label_text_layer, label_text_font);
  text_layer_set_overflow_mode(label_text_layer, text_layer_overflow_mode);
  text_layer_set_text_alignment(label_text_layer, text_layer_alignment);
  layer_add_child(window_root_layer, text_layer_get_layer(label_text_layer));

  GRect value_text_layer_frame = (GRect) {
    .size = GSize(text_container_frame.size.w, value_text_max_font_height),
  };
  grect_align(&value_text_layer_frame, &text_container_frame, GAlignBottom, true /* clip */);
  number_window->value_text_layer = text_layer_create(value_text_layer_frame);
  TextLayer *value_text_layer = number_window->value_text_layer;
  prv_update_value_text_layer(number_window);
  text_layer_set_text_color(value_text_layer, GColorBlack);
  text_layer_set_background_color(value_text_layer, GColorClear);
  text_layer_set_overflow_mode(value_text_layer, text_layer_overflow_mode);
  text_layer_set_text_alignment(value_text_layer, text_layer_alignment);
  layer_add_child(window_root_layer, text_layer_get_layer(value_text_layer));

  number_window->action_bar_layer = action_bar_layer_create();
  ActionBarLayer *action_bar_layer = number_window->action_bar_layer;
  action_bar_layer_set_click_config_provider(action_bar_layer, prv_click_config_provider);
  action_bar_layer_set_context(action_bar_layer, number_window);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, number_window->checkmark_icon);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, number_window->up_icon);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, number_window->down_icon);
  action_bar_layer_add_to_window(action_bar_layer, window);
}

static void prv_window_unload(Window *window) {
  GoalieNumberWindow *number_window = window_get_user_data(window);
  if (number_window) {
    text_layer_destroy(number_window->label_text_layer);
    text_layer_destroy(number_window->value_text_layer);
    action_bar_layer_destroy(number_window->action_bar_layer);
    gbitmap_destroy(number_window->checkmark_icon);
    gbitmap_destroy(number_window->up_icon);
    gbitmap_destroy(number_window->down_icon);
  }
}

static void prv_goalie_number_window_init(GoalieNumberWindow *number_window, const char *label,
                                          GoalieNumberWindowCallbacks callbacks,
                                          void *callback_context) {
  if (!number_window) {
    return;
  }

  *number_window = (GoalieNumberWindow) {
    .label = label,
    .value = 0,
    .min = GOALIE_NUMBER_WINDOW_MIN,
    .max = GOALIE_NUMBER_WINDOW_MAX,
    .step_size = 1,
    .callbacks = callbacks,
    .callback_context = callback_context,
  };

  number_window->window = window_create();
  Window *window = number_window->window;
  window_set_window_handlers(window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_set_background_color(window, GColorLightGray);
  window_set_user_data(window, number_window);
}

GoalieNumberWindow *goalie_number_window_create(const char *label,
                                                GoalieNumberWindowCallbacks callbacks,
                                                void *callback_context) {
  GoalieNumberWindow *number_window = calloc(1, sizeof(*number_window));
  prv_goalie_number_window_init(number_window, label, callbacks, callback_context);
  return number_window;
}

void goalie_number_window_destroy(GoalieNumberWindow *number_window) {
  free(number_window);
}

void goalie_number_window_set_label(GoalieNumberWindow *number_window, const char *label) {
  if (number_window) {
    text_layer_set_text(number_window->label_text_layer, label);
  }
}

void goalie_number_window_set_max(GoalieNumberWindow *number_window, int32_t max) {
  if (number_window && (max != number_window->max)) {
    number_window->max = MIN(max, GOALIE_NUMBER_WINDOW_MAX);
    goalie_number_window_set_value(number_window, number_window->value);
  }
}

void goalie_number_window_set_min(GoalieNumberWindow *number_window, int32_t min) {
  if (number_window && (min != number_window->min)) {
    number_window->min = MAX(min, GOALIE_NUMBER_WINDOW_MIN);
    goalie_number_window_set_value(number_window, number_window->value);
  }
}

void goalie_number_window_set_value(GoalieNumberWindow *number_window, int32_t value) {
  if (number_window &&
      (value != number_window->value) &&
      WITHIN(value, number_window->min, number_window->max)) {
    number_window->value = value;
    snprintf(number_window->value_text, GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH, "%"PRId32"",
             number_window->value);
    prv_update_value_text_layer(number_window);
  }
}

void goalie_number_window_set_step_size(GoalieNumberWindow *number_window, int32_t step) {
  if (number_window) {
    number_window->step_size = step;
  }
}

int32_t goalie_number_window_get_value(const GoalieNumberWindow *number_window) {
  if (!number_window) {
    return 0;
  }

  return number_window->value;
}

Window *goalie_number_window_get_window(GoalieNumberWindow *number_window) {
  if (!number_window) {
    return NULL;
  }

  return number_window->window;
}
