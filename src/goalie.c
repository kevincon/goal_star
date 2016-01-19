#include "goalie_configuration.h"
#include "goalie_progress_window.h"
#include "goalie_prompt_window.h"

#include <pebble.h>

static void prv_init(void) {
  goalie_configuration_init();
//  goalie_progress_window_push();
  goalie_prompt_window_push();
}

static void prv_deinit(void) {
  goalie_configuration_deinit();
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
