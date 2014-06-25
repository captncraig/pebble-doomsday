#include <pebble.h>

static Window *window;
static SimpleMenuLayer *simple_menu_layer;
static SimpleMenuSection menu_sections[1];
static SimpleMenuItem menu_items[3];

static int year_option_idx = 0;
const char *year_options[] = {"This Year Only","+/- 5 Years","+/- 30 years","1900-2030","1200-3200"};
static void change_year_option(int index, void *context){
  year_option_idx = (year_option_idx + 1) % ARRAY_LENGTH(year_options);
  menu_items[1].subtitle = year_options[year_option_idx];
  layer_mark_dirty(simple_menu_layer_get_layer(simple_menu_layer));
}

//Main game window
static Window *play_window;
static TextLayer* month_day;
static TextLayer* big_year;
static TextLayer* possible_options[7];
static InverterLayer* selection;
static int currentAnswer = 3;
const char *dows[] = {"S","M","T","W","T","F","S"};

static void answer(ClickRecognizerRef recognizer, void *context) {
}
static void cycle_selection_back(ClickRecognizerRef recognizer, void *context) {
  currentAnswer = (currentAnswer+6) % 7;
  Layer* l = inverter_layer_get_layer(selection);
  GRect bounds = layer_get_frame(l);
  layer_set_frame(l, GRect(currentAnswer*20, bounds.origin.y, bounds.size.w, bounds.size.h));
}
static void cycle_selection_forward(ClickRecognizerRef recognizer, void *context) {
  currentAnswer = (currentAnswer+1) % 7;
  Layer* l = inverter_layer_get_layer(selection);
  GRect bounds = layer_get_frame(l);
  layer_set_frame(l, GRect(currentAnswer*20, bounds.origin.y, bounds.size.w, bounds.size.h));
}
static void play_window_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)answer);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler)cycle_selection_back);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)cycle_selection_forward);
}

static void play_window_load(Window *window) {
  window_set_click_config_provider(window, (ClickConfigProvider) play_window_config_provider);
  Layer *window_layer = window_get_root_layer(window);

  month_day = text_layer_create(GRect(0, 10, 144, 42));
  text_layer_set_text(month_day, "FEB 13");
  text_layer_set_font(month_day, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(month_day, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(month_day));
  
  big_year = text_layer_create(GRect(0, 52, 144, 42));
  text_layer_set_text(big_year, "1976");
  text_layer_set_font(big_year, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(big_year, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(big_year));

  GRect bounds = layer_get_bounds(window_layer);
  for(int i = 0; i<7; i++){
    possible_options[i] = text_layer_create(GRect(i*(20), bounds.size.h - 24, 20, 18));
    text_layer_set_text(possible_options[i], dows[i]);
    text_layer_set_font(possible_options[i], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(possible_options[i], GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(possible_options[i]));
  }
  
  selection = inverter_layer_create(GRect(currentAnswer * 20,bounds.size.h - 24,20,24));
  layer_add_child(window_layer, inverter_layer_get_layer(selection));
}

static void play_window_unload(Window *window) {
  text_layer_destroy(month_day);
  text_layer_destroy(big_year);
  for(int i = 0; i<7; i++){
    text_layer_destroy(possible_options[i]);
  }
  inverter_layer_destroy(selection);
  window_destroy(play_window);
}

static void play(int index, void *context){
  play_window = window_create();
  window_set_window_handlers(play_window, (WindowHandlers) {
	  .load = play_window_load,
    .unload = play_window_unload,
  });
  window_stack_push(play_window, true);
}

//Main initialization
static void window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Load Main");
  menu_items[0] = (SimpleMenuItem){
    .title = "Play",
    .callback = play,
  };
  menu_items[1] = (SimpleMenuItem){
    .title = "Year Range",
    .subtitle = year_options[year_option_idx],
    .callback = change_year_option,
  };
  menu_sections[0] = (SimpleMenuSection){
    .num_items = 2,
    .items = menu_items,
  };
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  simple_menu_layer = simple_menu_layer_create(bounds, window, menu_sections, 1, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(simple_menu_layer));
}

static void window_unload(Window *window) {
  simple_menu_layer_destroy(simple_menu_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}