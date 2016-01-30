#pragma once

#include <pebble.h>

struct GoalieNumberWindow;
typedef struct GoalieNumberWindow GoalieNumberWindow;

typedef void (*GoalieNumberWindowCallback)(GoalieNumberWindow *number_window, void *context);

typedef struct {
  GoalieNumberWindowCallback selected;
} GoalieNumberWindowCallbacks;

GoalieNumberWindow *goalie_number_window_create(const char *label,
                                                GoalieNumberWindowCallbacks callbacks,
                                                void *callback_context);
void goalie_number_window_destroy(GoalieNumberWindow *number_window);
void goalie_number_window_set_label(GoalieNumberWindow *number_window, const char *label);
void goalie_number_window_set_max(GoalieNumberWindow *number_window, int32_t max);
void goalie_number_window_set_min(GoalieNumberWindow *number_window, int32_t min);
void goalie_number_window_set_value(GoalieNumberWindow *number_window, int32_t value);
void goalie_number_window_set_step_size(GoalieNumberWindow *number_window, int32_t step);
int32_t goalie_number_window_get_value(const GoalieNumberWindow *number_window);
Window *goalie_number_window_get_window(GoalieNumberWindow *number_window);
