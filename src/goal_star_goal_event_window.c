#include "goal_star_goal_event_window.h"

#include "../common/goal_star_configuration.h"

#include <pebble.h>

// Milliseconds between frames - 30 FPS max
#define ANIMATION_FRAME_INTERVAL_MS 33

#define WAIT_BEFORE_POP_MS 1000

typedef struct {
  Window *window;
  Layer *goal_reached_sequence_layer;
  GDrawCommandSequence *goal_reached_sequence;
  uint32_t goal_reached_sequence_frame_index;
  AppTimer *goal_reached_sequence_timer;
} GoalStarGoalEventWindowData;

static void prv_goal_reached_wait_timer_handler(void *context) {
  GoalStarGoalEventWindowData *data = context;
  if (data) {
    data->goal_reached_sequence_timer = NULL;
  }
  window_stack_pop(true /* animated */);
}

static void prv_goal_reached_sequence_layer_update_proc(Layer *layer, GContext *ctx) {
  GoalStarGoalEventWindowData *data = window_get_user_data(layer_get_window(layer));
  const GRect layer_bounds = layer_get_bounds(layer);

  GRect sequence_frame = (GRect) {
    .size = gdraw_command_sequence_get_bounds_size(data->goal_reached_sequence),
  };
  grect_align(&sequence_frame, &layer_bounds, GAlignCenter, true /* clip */);
  sequence_frame.origin.y -= sequence_frame.origin.y / 4;

  GDrawCommandFrame *frame = gdraw_command_sequence_get_frame_by_index(
    data->goal_reached_sequence, data->goal_reached_sequence_frame_index);
  if (frame) {
    gdraw_command_frame_draw(ctx, data->goal_reached_sequence, frame, sequence_frame.origin);
  }

  const uint32_t num_frames = gdraw_command_sequence_get_num_frames(data->goal_reached_sequence);
  if (++data->goal_reached_sequence_frame_index >= num_frames) {
    app_timer_cancel(data->goal_reached_sequence_timer);
    const uint32_t timeout_ms = goal_star_configuration_get_goal_event_timeout_ms();
    if (timeout_ms) {
      data->goal_reached_sequence_timer = app_timer_register(timeout_ms,
                                                             prv_goal_reached_wait_timer_handler,
                                                             data);
    }
  }

  char text[GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH] = {0};
  goal_star_configuration_get_goal_summary_string(text);
  graphics_context_set_text_color(ctx, GColorBlack);
  const GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  const int16_t font_height = 24;
  const GRect text_frame = GRect(0, sequence_frame.origin.y + sequence_frame.size.h,
                                 layer_bounds.size.w, font_height);
  graphics_draw_text(ctx, text, font, text_frame, GTextOverflowModeTrailingEllipsis,
                     GTextAlignmentCenter, NULL);
}

static void prv_goal_reached_sequence_timer_handler(void *context) {
  GoalStarGoalEventWindowData *data = context;
  if (data) {
    layer_mark_dirty(data->goal_reached_sequence_layer);
    data->goal_reached_sequence_timer = app_timer_register(ANIMATION_FRAME_INTERVAL_MS,
                                                           prv_goal_reached_sequence_timer_handler,
                                                           data);
  }
}

static void prv_vibrate(void) {
  const int32_t short_pulse_duration_ms = 100;
  const uint32_t vibe_segments[] = {
    // 4 short pulses
    short_pulse_duration_ms,
    short_pulse_duration_ms * 2,
    short_pulse_duration_ms,
    short_pulse_duration_ms * 2,
    short_pulse_duration_ms,
    short_pulse_duration_ms * 2,
    short_pulse_duration_ms,
    short_pulse_duration_ms * 2,
    // 1 long pulse
    short_pulse_duration_ms * 4,
    short_pulse_duration_ms * 2,
    // 3 short pulses
    short_pulse_duration_ms,
    short_pulse_duration_ms * 2,
    short_pulse_duration_ms,
    short_pulse_duration_ms * 2,
    short_pulse_duration_ms,
    short_pulse_duration_ms * 2,
  };
  VibePattern vibe_pattern = {
    .durations = vibe_segments,
    .num_segments = ARRAY_LENGTH(vibe_segments),
  };
  vibes_enqueue_custom_pattern(vibe_pattern);
}

static void prv_window_load(Window *window) {
  GoalStarGoalEventWindowData *data = window_get_user_data(window);
  if (!data) {
    return;
  }

  Layer *window_root_layer = window_get_root_layer(window);
  const GRect window_root_layer_bounds = layer_get_bounds(window_root_layer);

  data->goal_reached_sequence =
    gdraw_command_sequence_create_with_resource(RESOURCE_ID_GOAL_REACHED);

  data->goal_reached_sequence_layer = layer_create(window_root_layer_bounds);
  Layer *goal_reached_sequence_layer = data->goal_reached_sequence_layer;
  layer_set_update_proc(goal_reached_sequence_layer, prv_goal_reached_sequence_layer_update_proc);
  layer_add_child(window_root_layer, goal_reached_sequence_layer);

  data->goal_reached_sequence_timer = app_timer_register(ANIMATION_FRAME_INTERVAL_MS,
                                                         prv_goal_reached_sequence_timer_handler,
                                                         data);

  prv_vibrate();
  light_enable_interaction();
}

static void prv_window_unload(Window *window) {
  GoalStarGoalEventWindowData *data = window_get_user_data(window);

  if (data) {
    if (data->goal_reached_sequence_timer) {
      app_timer_cancel(data->goal_reached_sequence_timer);
    }
    layer_destroy(data->goal_reached_sequence_layer);
    gdraw_command_sequence_destroy(data->goal_reached_sequence);
    window_destroy(data->window);
  }

  free(data);
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop(true /* animated */);
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
}

void goal_star_goal_event_window_push(void) {
  GoalStarGoalEventWindowData *data = calloc(1, sizeof(*data));
  if (!data) {
    return;
  }

  data->window = window_create();
  Window *window = data->window;

  window_set_window_handlers(window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_set_click_config_provider(window, prv_click_config_provider);
  window_set_background_color(window, GColorPictonBlue);
  window_set_user_data(window, data);

  const bool animated = true;
  window_stack_push(window, animated);
}
