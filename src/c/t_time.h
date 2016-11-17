#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1
#define BUFFER_SIZE 86

const VibePattern SK_PATTERN = {
  .durations = (uint32_t []) {100, 200, 100, 200, 100, 200, 300, 200, 100, 200, 300},
  .num_segments = 11
};

const VibePattern CT_PATTERN = {
  .durations = (uint32_t []) {300, 200, 100, 200, 300, 200, 100, 200, 300},
  .num_segments = 9
};

//Days
static const char* const DAYS[] = {"Sunday",  "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//Months
static const char* const MONTHS[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// Structure for settings
typedef struct ClaySettings {
  int date_peek;
  int delay_peek;
  GColor bg_color;
  GColor fg_color;
  GColor dt_bg_color;
  GColor dt_fg_color;
  GColor bg_invert;
  GColor fg_invert;
  GColor fg_alert;
  GColor fg_chg_low;
  GColor fg_chg_med;
  GColor fg_chg_full;
} __attribute__((__packed__)) ClaySettings;

static void init_default_settings();
static void prv_save_settings();
static void handle_settings(DictionaryIterator *iter, void *context);
static void handle_battery(BatteryChargeState charge_state);
static void update_time(struct tm* t);
static void update_date(struct tm* t);
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed);
static void handle_bluetooth(bool connected);
static void timer_callback();
static void delay_callback();
static void handle_tap(AccelAxisType axis, int32_t direction);
static void do_init(void);
static void set_colors();
static void load_settings(void);
static void do_deinit(void);

