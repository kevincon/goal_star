#pragma once

#include <pebble.h>

#define GOAL_STAR_STATUS_BAR_LAYER_HEIGHT (PBL_IF_RECT_ELSE(18, 24))

typedef struct GoalStarStatusBarLayer {
  Layer *layer;
  AppTimer *timer;
} GoalStarStatusBarLayer;

void goal_star_status_bar_layer_init(GoalStarStatusBarLayer *status_bar_layer, int16_t width);

void goal_star_status_bar_layer_deinit(GoalStarStatusBarLayer *status_bar_layer);

void goal_star_status_bar_layer_set_colors(GoalStarStatusBarLayer *status_bar_layer,
                                           GColor background, GColor foreground);
