#include "../common/goal_star_configuration.h"
#include "goal_star_goal_event_window.h"
#include "goal_star_progress_window.h"
#include "goal_star_prompt_window.h"

#include <pebble.h>

static void prv_init(void) {
  goal_star_configuration_init();

  const AppLaunchReason app_launch_reason = launch_reason();
  switch (app_launch_reason) {
    case APP_LAUNCH_WORKER:
      goal_star_goal_event_window_push();
      break;
    default:
      if (app_worker_is_running()) {
        goal_star_progress_window_push();
      } else {
        goal_star_prompt_window_push();
      }
      break;
  }
}

static void prv_deinit(void) {
  goal_star_configuration_deinit();
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
