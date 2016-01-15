#include <pebble_worker.h>

static void prv_init(void) {

}

static void prv_deinit(void) {

}

int main(void) {
  prv_init();
  worker_event_loop();
  prv_deinit();
}
