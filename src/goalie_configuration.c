#include "goalie_configuration.h"

#define GOALIE_CONFIGURATION_CURRENT_VERSION 1

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

static void prv_set_default_configuration(GoalieConfiguration *configuration) {
  if (!configuration) {
    return;
  }

  *configuration = (GoalieConfiguration) {
    .goal_type = HealthMetricStepCount,
    .goal_value = 10000,
  };
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

GoalieConfiguration *goalie_configuration_get_configuration(void) {
  return &s_configuration;
}

void goalie_configuration_deinit(void) {
  persist_write_int(GoalieConfigurationPersistedDataKeys_Version,
                    GOALIE_CONFIGURATION_CURRENT_VERSION);
  persist_write_data(GoalieConfigurationPersistedDataKeys_Data, &s_configuration,
                     sizeof(s_configuration));
}
