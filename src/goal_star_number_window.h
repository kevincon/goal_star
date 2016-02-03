#pragma once

#include <pebble.h>

struct GoalStarNumberWindow;
typedef struct GoalStarNumberWindow GoalStarNumberWindow;

typedef void (*GoalStarNumberWindowCallback)(GoalStarNumberWindow *number_window, void *context);

typedef struct {
  GoalStarNumberWindowCallback selected;
} GoalStarNumberWindowCallbacks;

GoalStarNumberWindow *goal_star_number_window_create(const char *label,
                                                GoalStarNumberWindowCallbacks callbacks,
                                                void *callback_context);
void goal_star_number_window_destroy(GoalStarNumberWindow *number_window);
void goal_star_number_window_set_label(GoalStarNumberWindow *number_window, const char *label);
void goal_star_number_window_set_max(GoalStarNumberWindow *number_window, int32_t max);
void goal_star_number_window_set_min(GoalStarNumberWindow *number_window, int32_t min);
void goal_star_number_window_set_value(GoalStarNumberWindow *number_window, int32_t value);
void goal_star_number_window_set_step_size(GoalStarNumberWindow *number_window, int32_t step);
int32_t goal_star_number_window_get_value(const GoalStarNumberWindow *number_window);
Window *goal_star_number_window_get_window(GoalStarNumberWindow *number_window);
