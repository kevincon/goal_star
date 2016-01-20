#pragma once

#if GOALIE_WORKER
#include <pebble_worker.h>
#else
#include <pebble.h>
#endif

#define GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH 30
#define GOALIE_CONFIGURATION_APP_WORKER_MESSAGE_UPDATE_TYPE 0

void goalie_configuration_init(void);

HealthMetric goalie_configuration_get_goal_type(void);
HealthValue goalie_configuration_get_goal_value(void);

#if !GOALIE_WORKER
void goalie_configuration_deinit(void);
void goalie_configuration_get_goal_type_units_string(
  char result[GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH], bool all_caps);
void goalie_configuration_set_goal_type(HealthMetric new_goal_type);
void goalie_configuration_set_goal_value(HealthValue new_goal_value);
void goalie_configuration_get_goal_summary_string(
  char result[GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH]);
#endif
