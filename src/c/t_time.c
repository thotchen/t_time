#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86

const VibePattern SK_PATTERN = {
  .durations = (uint32_t []) {100, 200, 100, 200, 100, 200, 300, 200, 100, 200, 300},
  .num_segments = 11
};

const VibePattern CT_PATTERN = {
  .durations = (uint32_t []) {300, 200, 100, 200, 300, 200, 100, 200, 300},
  .num_segments = 9
};

static struct CommonWordsData {
  TextLayer *time;
  TextLayer *connection;
  TextLayer *battery;
  Window *window;
  char buffer[BUFFER_SIZE];
} s_data;

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "+%d%%", charge_state.charge_percent);
    text_layer_set_text_color(s_data.battery, GColorIcterine);
  } else if (charge_state.is_plugged) {
    snprintf(battery_text, sizeof(battery_text), "FULL");
    text_layer_set_text_color(s_data.battery, GColorGreen);
  } else {
    if (charge_state.charge_percent < 35) {
      text_layer_set_text_color(s_data.battery, GColorRed);
      snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
    } else {
      battery_text[0] = '\0';
    }
  }
  text_layer_set_text(s_data.battery, battery_text);

}

static void update_time(struct tm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, BUFFER_SIZE);
  text_layer_set_text(s_data.time, s_data.buffer);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  handle_battery(battery_state_service_peek());
}

static void handle_bluetooth(bool connected) {
  text_layer_set_text(s_data.connection, connected ? "" : "bluetooth disconnected");
  if (connected) {
    vibes_enqueue_custom_pattern(CT_PATTERN);
    window_set_background_color(s_data.window, GColorOxfordBlue);
  } else {
    vibes_enqueue_custom_pattern(SK_PATTERN);
    window_set_background_color(s_data.window, GColorElectricBlue);
  }
  
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorOxfordBlue);

  GFont large = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_38));
  GFont medium = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_20));
  GFont small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_16));

  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  s_data.time = text_layer_create(GRect(0, 10, frame.size.w, frame.size.h - 35));
  text_layer_set_background_color(s_data.time, GColorClear);
  text_layer_set_text_color(s_data.time, GColorIcterine);
  text_layer_set_font(s_data.time, large);
  text_layer_set_text_alignment(s_data.time, GTextAlignmentCenter);

  s_data.connection = text_layer_create(GRect(0, frame.size.h -35, frame.size.w, 35));
  text_layer_set_background_color(s_data.connection, GColorClear);
  text_layer_set_text_color(s_data.connection, GColorRed);
  text_layer_set_font(s_data.connection, small);
  text_layer_set_text_alignment(s_data.connection, GTextAlignmentCenter);

  s_data.battery = text_layer_create(GRect(frame.size.w -50, frame.size.h -30, 50, 30));
  text_layer_set_background_color(s_data.battery, GColorClear);
  text_layer_set_font(s_data.battery, medium);
  text_layer_set_text_alignment(s_data.battery, GTextAlignmentRight);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);

  layer_add_child(root_layer, text_layer_get_layer(s_data.time));
  layer_add_child(root_layer, text_layer_get_layer(s_data.connection));
  layer_add_child(root_layer, text_layer_get_layer(s_data.battery));

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);

}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.time);
  text_layer_destroy(s_data.connection);
  text_layer_destroy(s_data.battery);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
