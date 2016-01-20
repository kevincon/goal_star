#include "../common/goalie_configuration.h"
#include "goalie_goal_event_window.h"
#include "goalie_progress_window.h"
#include "goalie_prompt_window.h"

#include <pebble.h>

static void prv_init(void) {
  goalie_configuration_init();

  const AppLaunchReason app_launch_reason = launch_reason();
  switch (app_launch_reason) {
    case APP_LAUNCH_WORKER:
      goalie_goal_event_window_push();
      break;
    default:
      if (app_worker_is_running()) {
        goalie_progress_window_push();
      } else {
        goalie_prompt_window_push();
      }
      break;
  }
}

static void prv_deinit(void) {
  goalie_configuration_deinit();
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
