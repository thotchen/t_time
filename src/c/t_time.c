#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86
#define DATE_DISP_MS 1000

const VibePattern SK_PATTERN = {
  .durations = (uint32_t []) {100, 200, 100, 200, 100, 200, 300, 200, 100, 200, 300},
  .num_segments = 11
};

const VibePattern CT_PATTERN = {
  .durations = (uint32_t []) {300, 200, 100, 200, 300, 200, 100, 200, 300},
  .num_segments = 9
};

static const char* const DAYS[] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

static const char* const MONTHS[] = {
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};

static struct CommonWordsData {
  TextLayer *time;
  TextLayer *connection;
  TextLayer *battery;
  TextLayer *dateLayer;
  TextLayer *dateTextLayer;
  TextLayer *monthTextLayer;
  TextLayer *dayTextLayer;
  Window *window;
  char buffer[BUFFER_SIZE];
} s_data;

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "+%d%%", charge_state.charge_percent);
    text_layer_set_text_color(s_data.battery, PBL_IF_COLOR_ELSE(GColorIcterine, GColorWhite));
  } else if (charge_state.is_plugged) {
    snprintf(battery_text, sizeof(battery_text), "FULL");
    text_layer_set_text_color(s_data.battery, PBL_IF_COLOR_ELSE(GColorGreen, GColorWhite));
  } else {
    if (charge_state.charge_percent < 35) {
      text_layer_set_text_color(s_data.battery, PBL_IF_COLOR_ELSE(GColorRed, GColorWhite));
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

static void update_date(struct tm* t) {
  int date = t->tm_mday;
  static char date_text[] = "00";
  snprintf(date_text, sizeof(date_text), "%02d", date);
  text_layer_set_text(s_data.dateTextLayer,  date_text);
  int day = t->tm_wday;
  text_layer_set_text(s_data.dayTextLayer, DAYS[day]);
  int month = t->tm_mon;
  text_layer_set_text(s_data.monthTextLayer, MONTHS[month]);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  update_date(tick_time);
  handle_battery(battery_state_service_peek());
}

static void handle_bluetooth(bool connected) {
 text_layer_set_text(s_data.connection, connected ? "" : "bluetooth disconnected");
 if (connected) {
   vibes_enqueue_custom_pattern(CT_PATTERN);
   window_set_background_color(s_data.window, PBL_IF_COLOR_ELSE(GColorOxfordBlue, GColorBlack));
   text_layer_set_text_color(s_data.time, PBL_IF_COLOR_ELSE(GColorIcterine, GColorWhite));
  } else {
   vibes_enqueue_custom_pattern(SK_PATTERN);
   window_set_background_color(s_data.window, PBL_IF_COLOR_ELSE(GColorElectricBlue, GColorWhite));
   text_layer_set_text_color(s_data.time, PBL_IF_COLOR_ELSE(GColorPurple, GColorBlack));
  }
}

static void timer_callback(){
  layer_set_hidden((Layer *)s_data.dateLayer, true);
  layer_set_hidden((Layer *)s_data.dateTextLayer, true);
  layer_set_hidden((Layer *)s_data.dayTextLayer, true);
  layer_set_hidden((Layer *)s_data.monthTextLayer, true);
}

static void handle_tap(AccelAxisType axis, int32_t direction){
  app_timer_register(DATE_DISP_MS, timer_callback, NULL);
  layer_set_hidden((Layer *)s_data.dateLayer, false);
  layer_set_hidden((Layer *)s_data.dateTextLayer, false);
  layer_set_hidden((Layer *)s_data.dayTextLayer, false);
  layer_set_hidden((Layer *)s_data.monthTextLayer, false);
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, PBL_IF_COLOR_ELSE(GColorOxfordBlue, GColorBlack));

  GFont XL = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_48));
  GFont large = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_38));
  GFont medium = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_20));
  GFont small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_16));

  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  s_data.time = text_layer_create(GRect(0, 10, frame.size.w, frame.size.h - 35));
  text_layer_set_background_color(s_data.time, GColorClear);
  text_layer_set_text_color(s_data.time, PBL_IF_COLOR_ELSE(GColorIcterine, GColorWhite));
  text_layer_set_font(s_data.time, large);
  text_layer_set_text_alignment(s_data.time, GTextAlignmentCenter);

  s_data.connection = text_layer_create(GRect(0, frame.size.h -35, frame.size.w, 35));
  text_layer_set_background_color(s_data.connection, GColorClear);
  text_layer_set_text_color(s_data.connection, PBL_IF_COLOR_ELSE(GColorRed, GColorBlack));
  text_layer_set_font(s_data.connection, small);
  text_layer_set_text_alignment(s_data.connection, GTextAlignmentCenter);

  s_data.battery = text_layer_create(GRect(frame.size.w -50, frame.size.h -30, 50, 30));
  text_layer_set_background_color(s_data.battery, GColorClear);
  text_layer_set_font(s_data.battery, medium);
  text_layer_set_text_alignment(s_data.battery, GTextAlignmentRight);
  
  s_data.dateLayer = text_layer_create(frame);
  s_data.dateTextLayer = text_layer_create(GRect(0, frame.size.h / 3, frame.size.w, frame.size.h / 3));
  text_layer_set_background_color(s_data.dateLayer, PBL_IF_COLOR_ELSE(GColorOxfordBlue, GColorBlack));
  text_layer_set_background_color(s_data.dateTextLayer, GColorClear);
  text_layer_set_text_color(s_data.dateTextLayer, PBL_IF_COLOR_ELSE(GColorIcterine, GColorWhite));
  text_layer_set_font(s_data.dateTextLayer, XL);
  text_layer_set_text_alignment(s_data.dateTextLayer, GTextAlignmentCenter);
  
  s_data.dayTextLayer = text_layer_create(GRect(0, frame.size.h / 6, frame.size.w, frame.size.h / 6 ));
  text_layer_set_text_color(s_data.dayTextLayer, PBL_IF_COLOR_ELSE(GColorIcterine, GColorWhite));
  text_layer_set_background_color(s_data.dayTextLayer, GColorClear);
  text_layer_set_font(s_data.dayTextLayer, medium);
  text_layer_set_text_alignment(s_data.dayTextLayer, GTextAlignmentCenter);
  
  
  s_data.monthTextLayer = text_layer_create(GRect(0, 2 * frame.size.h / 3, frame.size.w, frame.size.h / 6 ));
  text_layer_set_text_color(s_data.monthTextLayer, PBL_IF_COLOR_ELSE(GColorIcterine, GColorWhite));
  text_layer_set_background_color(s_data.monthTextLayer, GColorClear);
  text_layer_set_font(s_data.monthTextLayer, medium);
  text_layer_set_text_alignment(s_data.monthTextLayer, GTextAlignmentCenter);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);
  update_date(t);

  layer_add_child(root_layer, text_layer_get_layer(s_data.time));
  layer_add_child(root_layer, text_layer_get_layer(s_data.connection));
  layer_add_child(root_layer, text_layer_get_layer(s_data.battery));
  layer_add_child(root_layer, text_layer_get_layer(s_data.dateLayer));
  layer_add_child(root_layer, text_layer_get_layer(s_data.dateTextLayer));
  layer_add_child(root_layer, text_layer_get_layer(s_data.dayTextLayer));
  layer_add_child(root_layer, text_layer_get_layer(s_data.monthTextLayer));
  layer_set_hidden((Layer *)s_data.dateLayer, true);
  layer_set_hidden((Layer *)s_data.dateTextLayer, true);
  layer_set_hidden((Layer *)s_data.dayTextLayer, true);
  layer_set_hidden((Layer *)s_data.monthTextLayer, true);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  accel_tap_service_subscribe(&handle_tap);

}

static void do_deinit(void) { 
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  accel_tap_service_unsubscribe();
  
  text_layer_destroy(s_data.time);
  text_layer_destroy(s_data.connection);
  text_layer_destroy(s_data.battery);
  text_layer_destroy(s_data.dateLayer);
  
  window_destroy(s_data.window);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
