#include "goalie_progress_window.h"

#include <pebble.h>

static void prv_init(void) {
  goalie_progress_window_push();
}

static void prv_deinit(void) {

}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
