#include "goalie_configuration.h"

#include <inttypes.h>

#define GOALIE_CONFIGURATION_CURRENT_VERSION 1

typedef struct {
  HealthMetric goal_type;
  HealthValue goal_value;
  uint32_t goal_event_timeout_ms;
  bool clock_time_enabled;
} GoalieConfiguration;

_Static_assert(sizeof(GoalieConfiguration) < PERSIST_DATA_MAX_LENGTH, "");

typedef enum {
  GoalieConfigurationPersistedDataKeys_Version,
  GoalieConfigurationPersistedDataKeys_Data,

  GoalieConfigurationPersistedDataKeys_Count
} GoalieConfigurationPersistedDataKeys;

static GoalieConfiguration s_configuration;

static void prv_clear_all_persisted_data(void) {
  for (uint32_t key = 0; key < GoalieConfigurationPersistedDataKeys_Count; key++) {
    persist_delete(key);
  }
}

static void prv_write_configuration(void) {
  persist_write_int(GoalieConfigurationPersistedDataKeys_Version,
                    GOALIE_CONFIGURATION_CURRENT_VERSION);
  persist_write_data(GoalieConfigurationPersistedDataKeys_Data, &s_configuration,
                     sizeof(s_configuration));

  // Just send an empty message to let the worker know to refresh the configuration
  AppWorkerMessage data = (AppWorkerMessage) {0};
  app_worker_send_message(GOALIE_CONFIGURATION_APP_WORKER_MESSAGE_UPDATE_TYPE, &data);
}

static void prv_set_default_configuration(GoalieConfiguration *configuration) {
  if (!configuration) {
    return;
  }

  *configuration = (GoalieConfiguration) {
    .goal_type = HealthMetricStepCount,
    .goal_value = 10000,
    .goal_event_timeout_ms = 0,
    .clock_time_enabled = true,
  };
  prv_write_configuration();
}

//! Returns true if data was migrated successfully and loaded into the static configuration
static bool prv_migrate_configuration_if_necessary(void) {
  if (!persist_exists(GoalieConfigurationPersistedDataKeys_Version)) {
    return false;
  }

  const uint32_t persisted_version = (uint32_t)persist_read_int(
    GoalieConfigurationPersistedDataKeys_Version);
  switch (persisted_version) {
    case GOALIE_CONFIGURATION_CURRENT_VERSION:
      // Valid version
      return false;
    // Add more cases here as versions increment to handle data migration; return true if migrated
    default:
      // Unexpected newer version, just clear all data and start over
      prv_clear_all_persisted_data();
      return false;
  }
}

HealthMetric goalie_configuration_get_goal_type(void) {
  return s_configuration.goal_type;
}

void goalie_configuration_get_goal_type_units_string(
  char result[GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH], bool all_caps) {
  if (!result) {
    return;
  }

  char *units_string = "???";
  switch (goalie_configuration_get_goal_type()) {
    case HealthMetricStepCount:
      units_string = all_caps ? "STEPS" : "steps";
      break;
    case HealthMetricWalkedDistanceMeters:
      units_string = all_caps ? "METERS" : "meters";
      break;
    default:
      break;
  }

  snprintf(result, GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH, "%s", units_string);
}

void goalie_configuration_set_goal_type(HealthMetric new_goal_type) {
  if (s_configuration.goal_type != new_goal_type) {
    s_configuration.goal_type = new_goal_type;
    prv_write_configuration();
  }
}

HealthValue goalie_configuration_get_goal_value(void) {
  return s_configuration.goal_value;
}

void goalie_configuration_set_goal_value(HealthValue new_goal_value) {
  if (s_configuration.goal_value != new_goal_value) {
    s_configuration.goal_value = new_goal_value;
    prv_write_configuration();
  }
}

void goalie_configuration_get_goal_summary_string(
  char result[GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH]) {
  if (!result) {
    return;
  }

  char units_string[GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH] = {0};
  goalie_configuration_get_goal_type_units_string(units_string, false /* all_caps */);

  snprintf(result, GOALIE_CONFIGURATION_STRING_BUFFER_LENGTH, "%"PRId32" %s!",
           goalie_configuration_get_goal_value(), units_string);
}

uint32_t goalie_configuration_get_goal_event_timeout_ms(void) {
  return s_configuration.goal_event_timeout_ms;
}

void goalie_configuration_set_goal_event_timeout_ms(uint32_t new_goal_event_timeout_ms) {
  if (s_configuration.goal_event_timeout_ms != new_goal_event_timeout_ms) {
    s_configuration.goal_event_timeout_ms = new_goal_event_timeout_ms;
    prv_write_configuration();
  }
}

bool goalie_configuration_get_clock_time_enabled(void) {
  return s_configuration.clock_time_enabled;
}

void goalie_configuration_set_clock_time_enabled(bool enabled) {
  if (s_configuration.clock_time_enabled != enabled) {
    s_configuration.clock_time_enabled = enabled;
    prv_write_configuration();
  }
}

void goalie_configuration_init(void) {
  if (prv_migrate_configuration_if_necessary()) {
    // Data was migrated and loaded into the static configuration, so we're good to go
    return;
  }

  if (persist_exists(GoalieConfigurationPersistedDataKeys_Data)) {
    persist_read_data(GoalieConfigurationPersistedDataKeys_Data, &s_configuration,
                      sizeof(s_configuration));
  } else {
    prv_set_default_configuration(&s_configuration);
  }
}

void goalie_configuration_deinit(void) {
  prv_write_configuration();
}
