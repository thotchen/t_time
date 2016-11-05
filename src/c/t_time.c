#include <pebble.h>
#include "t_time.h"
#include "num2words.h"

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

// Struct for settings (see t_time.h)
ClaySettings settings;

static void init_default_settings(){
  settings.date_peek = 1500;
  settings.bg_color = PBL_IF_COLOR_ELSE(GColorFromHEX(0x000055), GColorBlack);
  settings.fg_color = PBL_IF_COLOR_ELSE(GColorFromHEX(0xFFFF55), GColorWhite);
  settings.bg_invert = PBL_IF_COLOR_ELSE(GColorFromHEX(0x55FFFF), GColorWhite);
  settings.fg_invert = PBL_IF_COLOR_ELSE(GColorFromHEX(0xAA00AA), GColorBlack);
  settings.fg_alert = PBL_IF_COLOR_ELSE(GColorFromHEX(0xFF0000), GColorWhite);
  settings.fg_invert_alert = PBL_IF_COLOR_ELSE(GColorFromHEX(0xFF0000), GColorBlack);
  settings.fg_chg_full = PBL_IF_COLOR_ELSE(GColorFromHEX(0x00FF00), GColorWhite);
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  set_colors();
}

static void handle_settings(DictionaryIterator *iter, void *context) {
  // Read date preferences
  Tuple *date_peek_t = dict_find(iter, MESSAGE_KEY_datePeek);
  if(date_peek_t) {
    settings.date_peek = (date_peek_t->value->int32) * 100;
  }
  // Read color preferences
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_bg_color);
  if(bg_color_t) {
    settings.bg_color = GColorFromHEX(bg_color_t->value->int32);
  }
  Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_fg_color);
  if(bg_color_t) {
    settings.fg_color = GColorFromHEX(fg_color_t->value->int32);
  }
 
  prv_save_settings();
}

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";

  if (charge_state.is_charging) {
    snprintf(battery_text, sizeof(battery_text), "+%d%%", charge_state.charge_percent);
    if (charge_state.charge_percent < 35) {
      text_layer_set_text_color(s_data.battery, settings.fg_alert);
    } else if (charge_state.charge_percent < 70) {
      text_layer_set_text_color(s_data.battery, settings.fg_color);
    } else {
      text_layer_set_text_color(s_data.battery, settings.fg_chg_full);
    }
  } else if (charge_state.is_plugged) {
    snprintf(battery_text, sizeof(battery_text), "FULL");
    text_layer_set_text_color(s_data.battery, settings.fg_chg_full);
  } else {
    if (charge_state.charge_percent < 35) {
      text_layer_set_text_color(s_data.battery, settings.fg_alert);
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
   window_set_background_color(s_data.window, settings.bg_color);
   text_layer_set_text_color(s_data.time, settings.fg_color);
  } else {
   vibes_enqueue_custom_pattern(SK_PATTERN);
   window_set_background_color(s_data.window, settings.bg_invert);
   text_layer_set_text_color(s_data.time, settings.fg_invert);
  }
}

static void timer_callback(){
  layer_set_hidden((Layer *)s_data.dateLayer, true);
  layer_set_hidden((Layer *)s_data.dateTextLayer, true);
  layer_set_hidden((Layer *)s_data.dayTextLayer, true);
  layer_set_hidden((Layer *)s_data.monthTextLayer, true);
}

static void handle_tap(AccelAxisType axis, int32_t direction){
  app_timer_register(settings.date_peek, timer_callback, NULL);
  layer_set_hidden((Layer *)s_data.dateLayer, false);
  layer_set_hidden((Layer *)s_data.dateTextLayer, false);
  layer_set_hidden((Layer *)s_data.dayTextLayer, false);
  layer_set_hidden((Layer *)s_data.monthTextLayer, false);
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  //load fonts
  GFont XL = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_48));
  GFont large = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_38));
  GFont datefont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_24));
  GFont medium = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_20));
  GFont small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_QUIRKY_MESSY_16));

  //load layers
  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  s_data.time = text_layer_create(GRect(0, 10, frame.size.w, frame.size.h - 35));
  text_layer_set_background_color(s_data.time, GColorClear);
  text_layer_set_font(s_data.time, large);
  text_layer_set_text_alignment(s_data.time, GTextAlignmentCenter);

  s_data.connection = text_layer_create(GRect(0, frame.size.h -35, frame.size.w, 35));
  text_layer_set_background_color(s_data.connection, GColorClear);
  text_layer_set_font(s_data.connection, small);
  text_layer_set_text_alignment(s_data.connection, GTextAlignmentCenter);

  s_data.battery = text_layer_create(GRect(frame.size.w -50, frame.size.h -30, 50, 30));
  text_layer_set_background_color(s_data.battery, GColorClear);
  text_layer_set_font(s_data.battery, medium);
  text_layer_set_text_alignment(s_data.battery, GTextAlignmentRight);
  
  s_data.dateLayer = text_layer_create(frame);
  s_data.dateTextLayer = text_layer_create(GRect(0, frame.size.h / 3, frame.size.w, frame.size.h / 3));
  text_layer_set_background_color(s_data.dateTextLayer, GColorClear);
  text_layer_set_font(s_data.dateTextLayer, XL);
  text_layer_set_text_alignment(s_data.dateTextLayer, GTextAlignmentCenter);
  
  s_data.dayTextLayer = text_layer_create(GRect(0, frame.size.h / 6, frame.size.w, frame.size.h / 5 ));
  text_layer_set_background_color(s_data.dayTextLayer, GColorClear);
  text_layer_set_font(s_data.dayTextLayer, datefont);
  text_layer_set_text_alignment(s_data.dayTextLayer, GTextAlignmentCenter);
  
  
  s_data.monthTextLayer = text_layer_create(GRect(0, 2 * frame.size.h / 3, frame.size.w, frame.size.h / 6 ));
  text_layer_set_background_color(s_data.monthTextLayer, GColorClear);
  text_layer_set_font(s_data.monthTextLayer, datefont);
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
  
  app_message_register_inbox_received(handle_settings);
  app_message_open(128, 128);

}

static void set_colors(){
  //window background
  window_set_background_color(s_data.window, settings.bg_color);
  //time font
  text_layer_set_text_color(s_data.time, settings.fg_color);
  //blu tooth alert
  text_layer_set_text_color(s_data.connection, settings.fg_invert_alert);
  //date overlay background
  text_layer_set_background_color(s_data.dateLayer, settings.bg_color);
  //date font
  text_layer_set_text_color(s_data.dateTextLayer, settings.fg_color);
  text_layer_set_text_color(s_data.dayTextLayer, settings.fg_color);
  text_layer_set_text_color(s_data.monthTextLayer, settings.fg_color);  
}

static void load_settings(void){
  init_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
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
  load_settings();
  do_init();
  set_colors();
  app_event_loop();
  do_deinit();
}
