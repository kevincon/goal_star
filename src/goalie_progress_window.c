#include "goalie_progress_window.h"

#include "goalie_configuration.h"
#include "goalie_configuration_window.h"

#include <pebble.h>

#include <inttypes.h>

#define PROGRESS_VISUALIZATION_RADIAL_THICKNESS PBL_IF_RECT_ELSE(10, 15)
#define PROGRESS_GOAL_TEXT_MAX_STRING_LENGTH 6

typedef struct {
  Window *window;
  Layer *progress_visualization_layer;
  Layer *progress_text_layer;
} GoalieProgressWindowData;

static uint32_t prv_get_current_progress_towards_goal(void) {
  // TODO
  return 5280;
}

static uint32_t prv_get_goal(void) {
  // TODO
  return 10000;
}

static void prv_get_current_progress_towards_goal_as_string(
  char buffer[PROGRESS_GOAL_TEXT_MAX_STRING_LENGTH + 1]) {
  snprintf(buffer, PROGRESS_GOAL_TEXT_MAX_STRING_LENGTH + 1,
           "%"PRIu32"", prv_get_current_progress_towards_goal());
}

static void prv_progress_visualization_layer_update_proc(Layer *layer, GContext* ctx) {
  const GRect layer_bounds = layer_get_bounds(layer);

  const GOvalScaleMode oval_scale_mode = GOvalScaleModeFitCircle;

  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_radial(ctx, layer_bounds, oval_scale_mode, PROGRESS_VISUALIZATION_RADIAL_THICKNESS,
                       0, TRIG_MAX_ANGLE);

//  const HealthMetric goal_type = HealthMetricStepCount;
//  const time_t start_of_today = clock_to_timestamp(TODAY, 0, 0);
//  const time_t seconds_per_day = 60 * 60 * 24;
//  const time_t end_of_today = start_of_today + seconds_per_day - 1;
//  const HealthServiceAccessibilityMask goal_type_available =
//    health_service_metric_accessible(goal_type, start_of_today, end_of_today);

  const uint32_t current_progress = prv_get_current_progress_towards_goal();

  const GoalieConfiguration *configuration = goalie_configuration_get_configuration();
  const HealthValue goal = configuration->goal_value;

  const uint32_t progress_percentage = current_progress * 100 / goal;
  const int32_t progress_angle_end = progress_percentage * TRIG_MAX_ANGLE / 100;
  graphics_context_set_fill_color(ctx, GColorIslamicGreen);
  graphics_fill_radial(ctx, layer_bounds, oval_scale_mode, PROGRESS_VISUALIZATION_RADIAL_THICKNESS,
                       0, progress_angle_end);
}

static GRect prv_create_rect_aligned_inside_rect_with_height(const GRect *inside_rect,
                                                             int16_t height, GAlign alignment) {
  if (!inside_rect) {
    return GRectZero;
  }

  GRect rect = (GRect) {
    .size = GSize(inside_rect->size.w, height),
  };
  grect_align(&rect, inside_rect, alignment, true /* clip */);
  return rect;
}

static void prv_progress_text_layer_update_proc(Layer *layer, GContext* ctx) {
  const GRect layer_bounds = layer_get_bounds(layer);

  graphics_context_set_text_color(ctx, GColorBlack);
  const GTextOverflowMode text_overflow_mode = GTextOverflowModeTrailingEllipsis;
  const GTextAlignment text_alignment = GTextAlignmentCenter;

  const char *goal_progress_font_key = PBL_IF_RECT_ELSE(FONT_KEY_LECO_20_BOLD_NUMBERS,
                                                        FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM);
  const int16_t goal_progress_font_height = PBL_IF_RECT_ELSE(20, 26);
  const GFont goal_progress_font = fonts_get_system_font(goal_progress_font_key);

  const GFont goal_progress_type_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  const int16_t goal_progress_type_font_height= 18;

  const int16_t text_container_frame_height =
    goal_progress_font_height + goal_progress_type_font_height;
  GRect text_container_frame = prv_create_rect_aligned_inside_rect_with_height(
    &layer_bounds, text_container_frame_height, GAlignCenter);
  // Adjust for font cap heights
  text_container_frame.origin.y -= PBL_IF_RECT_ELSE(3, 4);

  // Draw the text for the current progress towards the goal
  char goal_progress_text[PROGRESS_GOAL_TEXT_MAX_STRING_LENGTH + 1] = {0};
  prv_get_current_progress_towards_goal_as_string(goal_progress_text);
  const GRect goal_progress_text_frame = prv_create_rect_aligned_inside_rect_with_height(
    &text_container_frame, goal_progress_font_height, GAlignTop);
  graphics_draw_text(ctx, goal_progress_text, goal_progress_font, goal_progress_text_frame,
                     text_overflow_mode, text_alignment, NULL);

  // Draw the text for the goal type label
  const GRect goal_progress_type_text_frame = prv_create_rect_aligned_inside_rect_with_height(
    &text_container_frame, goal_progress_type_font_height, GAlignBottom);
  graphics_draw_text(ctx, "STEPS", goal_progress_type_font, goal_progress_type_text_frame,
                     text_overflow_mode, text_alignment, NULL);
}
static GRect prv_get_rect_inscribed_in_circle_circumscribed_in_rect(
  const GRect *circumscribed_in_rect, int16_t circle_inset_thickness) {
  if (!circumscribed_in_rect) {
    return GRectZero;
  }

  const int16_t radius =
    (circumscribed_in_rect->size.w / 2) - circle_inset_thickness;

  // Approximation of (2 * radius) / sqrt(2)
  const int16_t frame_side = (2 * radius) * 408 / 577;

  GRect rect = (GRect) {
    .size = GSize(frame_side, frame_side),
  };
  grect_align(&rect, circumscribed_in_rect, GAlignCenter, true /* clip */);
  return rect;
}

static void prv_window_load(Window *window) {
  GoalieProgressWindowData *data = window_get_user_data(window);
  if (!data) {
    return;
  }

  Layer *window_root_layer = window_get_root_layer(window);
  const GRect window_root_layer_bounds = layer_get_bounds(window_root_layer);

  GRect progress_visualization_layer_frame = window_root_layer_bounds;
#if PBL_RECT
  const int16_t back_off_from_edge_inset = 2;
  progress_visualization_layer_frame = grect_inset(window_root_layer_bounds,
                                                   GEdgeInsets(back_off_from_edge_inset));
#endif
  data->progress_visualization_layer = layer_create(progress_visualization_layer_frame);
  Layer *progress_visualization_layer = data->progress_visualization_layer;
  layer_set_update_proc(progress_visualization_layer, prv_progress_visualization_layer_update_proc);
  layer_add_child(window_root_layer, progress_visualization_layer);

  const GRect progress_text_layer_frame = prv_get_rect_inscribed_in_circle_circumscribed_in_rect(
    &progress_visualization_layer_frame, PROGRESS_VISUALIZATION_RADIAL_THICKNESS);
  data->progress_text_layer = layer_create(progress_text_layer_frame);
  Layer *progress_text_layer = data->progress_text_layer;
  layer_set_update_proc(progress_text_layer, prv_progress_text_layer_update_proc);
  layer_add_child(window_root_layer, progress_text_layer);
}

static void prv_window_unload(Window *window) {
  GoalieProgressWindowData *data = window_get_user_data(window);

  if (data) {
    layer_destroy(data->progress_text_layer);
    layer_destroy(data->progress_visualization_layer);
    window_destroy(data->window);
  }

  free(data);
}

static void prv_window_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  goalie_configuration_window_push();
}

static void prv_window_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_window_select_click_handler);
}

void goalie_progress_window_push(void) {
  GoalieProgressWindowData *data = calloc(1, sizeof(*data));
  if (!data) {
    return;
  }

  data->window = window_create();
  Window *window = data->window;

  window_set_window_handlers(window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_set_click_config_provider(window, prv_window_click_config_provider);
  window_set_user_data(window, data);

  const bool animated = true;
  window_stack_push(window, animated);
}
