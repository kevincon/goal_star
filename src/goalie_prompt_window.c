#include "goalie_prompt_window.h"

#include "goalie_progress_window.h"

#include <pebble.h>

typedef struct {
  Window *window;
  Layer *text_layer;
  ActionBarLayer *action_bar_layer;
  GBitmap *checkmark_icon;
} GoaliePromptWindowData;

static void prv_text_layer_update_proc(Layer *layer, GContext* ctx) {
  GTextAttributes *text_attributes = graphics_text_attributes_create();
  graphics_text_attributes_enable_screen_text_flow(text_attributes, 4);

  const char *text = "Click select to make Goalie the background app.\n\nGoalie must run in the "
                     "background to notify you when you reach your goal.";

  const GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  const GTextAlignment text_alignment = PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentRight);
  graphics_draw_text(ctx, text, font, layer_get_bounds(layer), GTextOverflowModeTrailingEllipsis,
                     text_alignment, text_attributes);
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  GoaliePromptWindowData *data = context;
  const AppWorkerResult result = app_worker_launch();
  const bool animated = false;
  switch (result) {
    case APP_WORKER_RESULT_ALREADY_RUNNING:
    case APP_WORKER_RESULT_ASKING_CONFIRMATION:
    case APP_WORKER_RESULT_SUCCESS:
      goalie_progress_window_push();
      window_stack_remove(data->window, animated);
      break;
    case APP_WORKER_RESULT_DIFFERENT_APP:
    case APP_WORKER_RESULT_NO_WORKER:
    case APP_WORKER_RESULT_NOT_RUNNING:
      goalie_prompt_window_push();
      window_stack_remove(data->window, animated);
      break;
    default:
      window_stack_pop(animated);
      break;
  }
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_set_click_context(BUTTON_ID_SELECT, context);
}

static void prv_window_load(Window *window) {
  GoaliePromptWindowData *data = window_get_user_data(window);
  if (!data) {
    return;
  }

  Layer *window_root_layer = window_get_root_layer(window);
  const int16_t horizontal_padding = 4;
  const int16_t right_inset = ACTION_BAR_WIDTH + horizontal_padding;
  const int16_t top_inset = PBL_IF_RECT_ELSE(0, 15);
  const GRect text_layer_frame = grect_inset(layer_get_bounds(window_root_layer),
                                             GEdgeInsets(top_inset, right_inset, 0,
                                                         horizontal_padding));

  data->text_layer = layer_create(text_layer_frame);
  Layer *text_layer = data->text_layer;
  layer_set_update_proc(data->text_layer, prv_text_layer_update_proc);
  layer_add_child(window_root_layer, text_layer);

  data->checkmark_icon = gbitmap_create_with_resource(RESOURCE_ID_CHECKMARK);

  data->action_bar_layer = action_bar_layer_create();
  ActionBarLayer *action_bar_layer = data->action_bar_layer;
  action_bar_layer_set_click_config_provider(action_bar_layer, prv_click_config_provider);
  action_bar_layer_set_context(action_bar_layer, data);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, data->checkmark_icon);
  action_bar_layer_add_to_window(action_bar_layer, window);
}

static void prv_window_unload(Window *window) {
  GoaliePromptWindowData *data = window_get_user_data(window);

  if (data) {
    layer_destroy(data->text_layer);
    action_bar_layer_destroy(data->action_bar_layer);
    gbitmap_destroy(data->checkmark_icon);
    window_destroy(data->window);
  }

  free(data);
}

void goalie_prompt_window_push(void) {
  GoaliePromptWindowData *data = calloc(1, sizeof(*data));
  if (!data) {
    return;
  }

  data->window = window_create();
  Window *window = data->window;

  window_set_window_handlers(window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_set_background_color(window, GColorCobaltBlue);
  window_set_user_data(window, data);

  const bool animated = true;
  window_stack_push(window, animated);
}
