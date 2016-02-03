#pragma once

#if GOAL_STAR_WORKER
#include <pebble_worker.h>
#else
#include <pebble.h>
#endif

#define WITHIN(x, a, b) (((x) >= (a)) && ((x) <= (b)))
#define MIN(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __a : __b; })
#define MAX(A,B)    ({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })

#define GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH 30
#define GOAL_STAR_CONFIGURATION_APP_WORKER_MESSAGE_UPDATE_TYPE 0

void goal_star_configuration_init(void);

HealthMetric goal_star_configuration_get_goal_type(void);
HealthValue goal_star_configuration_get_goal_value(void);
uint32_t goal_star_configuration_get_goal_event_timeout_ms(void);

#if !GOAL_STAR_WORKER
void goal_star_configuration_deinit(void);
void goal_star_configuration_get_goal_type_units_string(
  char result[GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH], bool all_caps);
void goal_star_configuration_set_goal_type(HealthMetric new_goal_type);
void goal_star_configuration_set_goal_value(HealthValue new_goal_value);
void goal_star_configuration_set_goal_event_timeout_ms(uint32_t new_goal_event_timeout_ms);
bool goal_star_configuration_get_clock_time_enabled(void);
void goal_star_configuration_set_clock_time_enabled(bool enabled);
void goal_star_configuration_get_goal_summary_string(
  char result[GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH]);
#endif
