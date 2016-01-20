#include <pebble_worker.h>

#define GOALIE_WORKER 1

#include "../common/goalie_configuration.h"

static HealthValue s_last_health_value_recorded;

static void prv_health_event_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate) {
    const HealthMetric goal_type = goalie_configuration_get_goal_type();
    const HealthValue current_progress = health_service_sum_today(goal_type);
    const HealthValue goal = goalie_configuration_get_goal_value();
    if (s_last_health_value_recorded < goal && current_progress >= goal) {
      worker_launch_app();
    }
    s_last_health_value_recorded = current_progress;
  }
}

// TODO need to re-init config if it changes; need app to send the worker a message if it does

static void prv_init(void) {
  goalie_configuration_init();
  health_service_events_subscribe(prv_health_event_handler, NULL);
}

static void prv_deinit(void) {
  health_service_events_unsubscribe();
}

int main(void) {
  prv_init();
  worker_event_loop();
  prv_deinit();
}
