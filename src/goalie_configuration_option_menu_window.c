#include "goalie_configuration_option_menu_window.h"

#include <pebble.h>

typedef struct {
  Window *window;
#if PBL_RECT
  TextLayer *title_layer;
#endif
  MenuLayer *menu_layer;
  GoalieConfigurationOptionMenuWindowSettings settings;
} GoalieConfigurationOptionMenuWindowData;

static uint16_t prv_menu_layer_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index,
                                                     void *context) {
  GoalieConfigurationOptionMenuWindowData *data = context;
  return data->settings.callbacks.get_num_choices();
}

static void prv_menu_layer_draw_row_callback(GContext *ctx, const Layer *cell_layer,
                                             MenuIndex *cell_index, void *context) {
  GoalieConfigurationOptionMenuWindowData *data = context;
  char title[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH] = {0};
  data->settings.callbacks.get_string_for_index(cell_index->row, title);
  menu_cell_basic_draw(ctx, cell_layer, title, NULL, NULL);
}

#if PBL_ROUND
static int16_t prv_menu_layer_get_cell_height(MenuLayer *menu_layer, MenuIndex *cell_index,
                                              void *data) {
  if (menu_layer_is_index_selected(menu_layer, cell_index)) {
    return MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT;
  } else {
    return MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT;
  }
}
#endif

static void prv_menu_layer_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index,
                                           void *context) {
  GoalieConfigurationOptionMenuWindowData *data = context;
  if (data->settings.callbacks.choice_made) {
    data->settings.callbacks.choice_made(cell_index->row);
  }
  const bool animated = true;
  window_stack_pop(animated);
}

static void prv_window_load(Window *window) {
  GoalieConfigurationOptionMenuWindowData *data = window_get_user_data(window);
  if (!data) {
    return;
  }

  Layer *window_root_layer = window_get_root_layer(window);
  const GRect window_root_layer_bounds = layer_get_bounds(window_root_layer);
#if PBL_RECT
  const GRect title_layer_frame = (GRect) {
    // Adjust for font cap offset
    .origin = GPoint(0, -2),
    .size = GSize(window_root_layer_bounds.size.w, STATUS_BAR_LAYER_HEIGHT),
  };
  data->title_layer = text_layer_create(title_layer_frame);
  TextLayer *title_layer = data->title_layer;
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(title_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_background_color(title_layer, GColorClear);
  text_layer_set_text_color(title_layer, GColorBlack);
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(title_layer, data->settings.option_title);
  layer_add_child(window_root_layer, text_layer_get_layer(title_layer));
#endif

  const GEdgeInsets menu_layer_insets = PBL_IF_RECT_ELSE(GEdgeInsets(STATUS_BAR_LAYER_HEIGHT, 0, 0),
                                                         GEdgeInsets(STATUS_BAR_LAYER_HEIGHT, 0));
  const GRect menu_layer_frame = grect_inset(window_root_layer_bounds, menu_layer_insets);
  data->menu_layer = menu_layer_create(menu_layer_frame);
  MenuLayer *menu_layer = data->menu_layer;
  menu_layer_set_callbacks(menu_layer, data, (MenuLayerCallbacks) {
    .get_num_rows = prv_menu_layer_get_num_rows_callback,
    .draw_row = prv_menu_layer_draw_row_callback,
#if PBL_ROUND
    .get_cell_height = prv_menu_layer_get_cell_height,
#endif
    .select_click = prv_menu_layer_select_callback,
  });
  menu_layer_set_normal_colors(menu_layer, GColorWhite, GColorBlack);
  menu_layer_set_highlight_colors(menu_layer, GColorCobaltBlue, GColorWhite);
  menu_layer_set_click_config_onto_window(menu_layer, window);
  layer_add_child(window_root_layer, menu_layer_get_layer(menu_layer));
}

static void prv_window_unload(Window *window) {
  GoalieConfigurationOptionMenuWindowData *data = window_get_user_data(window);

  if (data) {
    menu_layer_destroy(data->menu_layer);
#if PBL_RECT
    text_layer_destroy(data->title_layer);
#endif
    window_destroy(data->window);
  }

  free(data);
}

void goalie_configuration_option_menu_window_push(
  const GoalieConfigurationOptionMenuWindowSettings *settings) {
  if (!settings) {
    return;
  }

  GoalieConfigurationOptionMenuWindowData *data = calloc(1, sizeof(*data));
  if (!data) {
    return;
  }

  data->settings = *settings;

  data->window = window_create();
  Window *window = data->window;

  window_set_window_handlers(window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_set_user_data(window, data);

  const bool animated = true;
  window_stack_push(window, animated);
}
