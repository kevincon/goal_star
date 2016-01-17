#pragma once

#include <pebble.h>

typedef struct {
  HealthMetric goal_type;
  HealthValue goal_value;
} GoalieConfiguration;

_Static_assert(sizeof(GoalieConfiguration) < PERSIST_DATA_MAX_LENGTH, "");

void goalie_configuration_init(void);
GoalieConfiguration *goalie_configuration_get_configuration(void);
void goalie_configuration_deinit(void);
