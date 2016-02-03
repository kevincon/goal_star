#include <pebble_worker.h>

#define GOAL_STAR_WORKER 1

#include "../common/goal_star_configuration.h"

static HealthValue s_last_health_value_recorded;

static void prv_health_event_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate) {
    const HealthMetric goal_type = goal_star_configuration_get_goal_type();
    const HealthValue current_progress = health_service_sum_today(goal_type);
    const HealthValue goal = goal_star_configuration_get_goal_value();
    if (s_last_health_value_recorded < goal && current_progress >= goal) {
      worker_launch_app();
    }
    s_last_health_value_recorded = current_progress;
  }
}

static void prv_message_handler(uint16_t type, AppWorkerMessage *data) {
  if (type == GOAL_STAR_CONFIGURATION_APP_WORKER_MESSAGE_UPDATE_TYPE) {
    const HealthMetric previous_goal_type = goal_star_configuration_get_goal_type();

    // Refresh the configuration
    goal_star_configuration_init();

    const HealthMetric new_goal_type = goal_star_configuration_get_goal_type();

    if (previous_goal_type != new_goal_type) {
      s_last_health_value_recorded = health_service_sum_today(new_goal_type);
    }
  }
}
static void prv_init(void) {
  goal_star_configuration_init();
  health_service_events_subscribe(prv_health_event_handler, NULL);
  app_worker_message_subscribe(prv_message_handler);
}

static void prv_deinit(void) {
  app_worker_message_unsubscribe();
  health_service_events_unsubscribe();
}

int main(void) {
  prv_init();
  worker_event_loop();
  prv_deinit();
}
