#include "goal_star_status_bar_layer.h"

//! Oh hey Nyquist–Shannon!
#define TIMER_TIMEOUT_MS (500)

static void prv_timer_callback(void *data) {
  GoalStarStatusBarLayer *status_bar_layer = data;
  if (!status_bar_layer) {
    return;
  }

  layer_mark_dirty(status_bar_layer->layer);

  status_bar_layer->timer = app_timer_register(TIMER_TIMEOUT_MS, prv_timer_callback,
                                               status_bar_layer);
}

static void prv_layer_update_proc(Layer *layer, GContext *ctx) {
  GoalStarStatusBarLayer *status_bar_layer = (GoalStarStatusBarLayer *)layer;
  if (!status_bar_layer) {
    return;
  }

  char clock_text[10] = {0};
  clock_copy_time_string(clock_text, sizeof(clock_text));

  GFont font = fonts_get_system_font(PBL_IF_RECT_ELSE(FONT_KEY_GOTHIC_18_BOLD,
                                                      FONT_KEY_GOTHIC_24_BOLD));

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, clock_text, font, layer_get_bounds(layer),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

void goal_star_status_bar_layer_init(GoalStarStatusBarLayer *status_bar_layer, int16_t width) {
  if (!status_bar_layer) {
    return;
  }

  const GRect frame = GRect(0, 0, width, GOAL_STAR_STATUS_BAR_LAYER_HEIGHT);
  Layer *layer = layer_create(frame);
  layer_set_update_proc(layer, prv_layer_update_proc);

  *status_bar_layer = (GoalStarStatusBarLayer) {
    .layer = layer,
    .timer = app_timer_register(TIMER_TIMEOUT_MS, prv_timer_callback, status_bar_layer),
  };
}

void goal_star_status_bar_layer_deinit(GoalStarStatusBarLayer *status_bar_layer) {
  if (!status_bar_layer) {
    return;
  }

  app_timer_cancel(status_bar_layer->timer);
  layer_destroy(status_bar_layer->layer);
}
