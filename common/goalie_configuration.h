#pragma once

#if GOALIE_WORKER
#include <pebble_worker.h>
#else
#include <pebble.h>
#endif

#define GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH 30

void goalie_configuration_init(void);
void goalie_configuration_deinit(void);

HealthMetric goalie_configuration_get_goal_type(void);
void goalie_configuration_get_goal_type_units_string(
  char result[GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH], bool all_caps);
void goalie_configuration_set_goal_type(HealthMetric new_goal_type);

HealthValue goalie_configuration_get_goal_value(void);
void goalie_configuration_set_goal_value(HealthValue new_goal_value);

void goalie_configuration_get_goal_summary_string(
  char result[GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH]);
