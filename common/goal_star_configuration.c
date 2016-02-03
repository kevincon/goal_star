#include "goal_star_configuration.h"

#include <inttypes.h>

#define GOAL_STAR_CONFIGURATION_CURRENT_VERSION 1

typedef struct {
  HealthMetric goal_type;
  HealthValue goal_value;
  uint32_t goal_event_timeout_ms;
  bool clock_time_enabled;
} GoalStarConfiguration;

_Static_assert(sizeof(GoalStarConfiguration) < PERSIST_DATA_MAX_LENGTH, "");

typedef enum {
  GoalStarConfigurationPersistedDataKeys_Version,
  GoalStarConfigurationPersistedDataKeys_Data,

  GoalStarConfigurationPersistedDataKeys_Count
} GoalStarConfigurationPersistedDataKeys;

static GoalStarConfiguration s_configuration;

static void prv_clear_all_persisted_data(void) {
  for (uint32_t key = 0; key < GoalStarConfigurationPersistedDataKeys_Count; key++) {
    persist_delete(key);
  }
}

static void prv_write_configuration(void) {
  persist_write_int(GoalStarConfigurationPersistedDataKeys_Version,
                    GOAL_STAR_CONFIGURATION_CURRENT_VERSION);
  persist_write_data(GoalStarConfigurationPersistedDataKeys_Data, &s_configuration,
                     sizeof(s_configuration));

  // Just send an empty message to let the worker know to refresh the configuration
  AppWorkerMessage data = (AppWorkerMessage) {0};
  app_worker_send_message(GOAL_STAR_CONFIGURATION_APP_WORKER_MESSAGE_UPDATE_TYPE, &data);
}

static void prv_set_default_configuration(GoalStarConfiguration *configuration) {
  if (!configuration) {
    return;
  }

  *configuration = (GoalStarConfiguration) {
    .goal_type = HealthMetricStepCount,
    .goal_value = 10000,
    .goal_event_timeout_ms = 0,
    .clock_time_enabled = true,
  };
  prv_write_configuration();
}

//! Returns true if data was migrated successfully and loaded into the static configuration
static bool prv_migrate_configuration_if_necessary(void) {
  if (!persist_exists(GoalStarConfigurationPersistedDataKeys_Version)) {
    return false;
  }

  const uint32_t persisted_version = (uint32_t)persist_read_int(
    GoalStarConfigurationPersistedDataKeys_Version);
  switch (persisted_version) {
    case GOAL_STAR_CONFIGURATION_CURRENT_VERSION:
      // Valid version
      return false;
    // Add more cases here as versions increment to handle data migration; return true if migrated
    default:
      // Unexpected newer version, just clear all data and start over
      prv_clear_all_persisted_data();
      return false;
  }
}

HealthMetric goal_star_configuration_get_goal_type(void) {
  return s_configuration.goal_type;
}

void goal_star_configuration_get_goal_type_units_string(
  char result[GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH], bool all_caps) {
  if (!result) {
    return;
  }

  char *units_string = "???";
  switch (goal_star_configuration_get_goal_type()) {
    case HealthMetricStepCount:
      units_string = all_caps ? "STEPS" : "steps";
      break;
    case HealthMetricWalkedDistanceMeters:
      units_string = all_caps ? "METERS" : "meters";
      break;
    default:
      break;
  }

  snprintf(result, GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH, "%s", units_string);
}

void goal_star_configuration_set_goal_type(HealthMetric new_goal_type) {
  if (s_configuration.goal_type != new_goal_type) {
    s_configuration.goal_type = new_goal_type;
    prv_write_configuration();
  }
}

HealthValue goal_star_configuration_get_goal_value(void) {
  return s_configuration.goal_value;
}

void goal_star_configuration_set_goal_value(HealthValue new_goal_value) {
  if (s_configuration.goal_value != new_goal_value) {
    s_configuration.goal_value = new_goal_value;
    prv_write_configuration();
  }
}

void goal_star_configuration_get_goal_summary_string(
  char result[GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH]) {
  if (!result) {
    return;
  }

  char units_string[GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH] = {0};
  goal_star_configuration_get_goal_type_units_string(units_string, false /* all_caps */);

  snprintf(result, GOAL_STAR_CONFIGURATION_STRING_BUFFER_LENGTH, "%"PRId32" %s!",
           goal_star_configuration_get_goal_value(), units_string);
}

uint32_t goal_star_configuration_get_goal_event_timeout_ms(void) {
  return s_configuration.goal_event_timeout_ms;
}

void goal_star_configuration_set_goal_event_timeout_ms(uint32_t new_goal_event_timeout_ms) {
  if (s_configuration.goal_event_timeout_ms != new_goal_event_timeout_ms) {
    s_configuration.goal_event_timeout_ms = new_goal_event_timeout_ms;
    prv_write_configuration();
  }
}

bool goal_star_configuration_get_clock_time_enabled(void) {
  return s_configuration.clock_time_enabled;
}

void goal_star_configuration_set_clock_time_enabled(bool enabled) {
  if (s_configuration.clock_time_enabled != enabled) {
    s_configuration.clock_time_enabled = enabled;
    prv_write_configuration();
  }
}

void goal_star_configuration_init(void) {
  if (prv_migrate_configuration_if_necessary()) {
    // Data was migrated and loaded into the static configuration, so we're good to go
    return;
  }

  if (persist_exists(GoalStarConfigurationPersistedDataKeys_Data)) {
    persist_read_data(GoalStarConfigurationPersistedDataKeys_Data, &s_configuration,
                      sizeof(s_configuration));
  } else {
    prv_set_default_configuration(&s_configuration);
  }
}

void goal_star_configuration_deinit(void) {
  prv_write_configuration();
}
