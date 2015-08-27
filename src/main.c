/*
 * main.c
 * Sets up a Window object and pushes it onto the stack.
 */

#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static GBitmap *s_city_bitmap_black, *s_city_bitmap_white, *s_airplane_bitmap, *s_clouds_bitmap;
static Layer *s_background_layer, *s_airplane_layer, *s_clouds_layer;
static InverterLayer *s_invert_layer;
int airplane_height;
bool daytime = false;

static void draw_city(Layer *layer, GContext *ctx) {
  // Draw white
  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  graphics_draw_bitmap_in_rect(ctx, s_city_bitmap_white, gbitmap_get_bounds(s_city_bitmap_white));

  // Draw black
  graphics_context_set_compositing_mode(ctx, GCompOpClear);
  graphics_draw_bitmap_in_rect(ctx, s_city_bitmap_black, gbitmap_get_bounds(s_city_bitmap_black));
}

static void draw_airplane(Layer *layer, GContext *ctx) {
  // Draw white
  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  graphics_draw_bitmap_in_rect(ctx, s_airplane_bitmap, gbitmap_get_bounds(s_airplane_bitmap));

  // Draw black
  //graphics_context_set_compositing_mode(ctx, GCompOpClear);
  //graphics_draw_bitmap_in_rect(ctx, s_black_bitmap, gbitmap_get_bounds(s_black_bitmap));
}

static void draw_clouds(Layer *layer, GContext *ctx) {
  // Draw white
  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  graphics_draw_bitmap_in_rect(ctx, s_clouds_bitmap, gbitmap_get_bounds(s_clouds_bitmap));

  // Draw black
  //graphics_context_set_compositing_mode(ctx, GCompOpClear);
  //graphics_draw_bitmap_in_rect(ctx, s_black_bitmap, gbitmap_get_bounds(s_black_bitmap));
}

static void main_window_load(Window *window) {
  // Create Window's child Layers here
  
  // City Background
  s_city_bitmap_black = gbitmap_create_with_resource(RESOURCE_ID_CITY_BACKGROUND_BLACK);
  s_city_bitmap_white = gbitmap_create_with_resource(RESOURCE_ID_CITY_BACKGROUND_WHITE);
  s_background_layer = layer_create(GRect(0, 20, 144, 104));
  layer_set_update_proc(s_background_layer, draw_city);
  layer_add_child(window_get_root_layer(window), s_background_layer);
  
  // Airplane
  s_airplane_bitmap = gbitmap_create_with_resource(RESOURCE_ID_AIRPLANE_WHITE);
  s_airplane_layer = layer_create(GRect(0, 10, 10, 3));
  layer_set_update_proc(s_airplane_layer, draw_airplane);
  layer_add_child(window_get_root_layer(window), s_airplane_layer);
  
  // Clouds
  s_clouds_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CLOUDS_WHITE);
  s_clouds_layer = layer_create(GRect(12, 25, 144, 32));
  layer_set_update_proc(s_clouds_layer, draw_clouds);
  layer_add_child(window_get_root_layer(window), s_clouds_layer);
  
  //Time display
  s_time_layer = text_layer_create(GRect(0, 112, 144, 58));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DIGITAL_KAUNO_FONT_65));
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //Invert for daytime
  s_invert_layer = inverter_layer_create(GRect(0, 0, 144, 168));
}

static void update_time() {
  //get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  if (daytime && (tick_time->tm_hour >= 18 || tick_time->tm_hour < 6)) {
    layer_remove_from_parent(inverter_layer_get_layer(s_invert_layer));
    daytime = false;
  } else if (!daytime && tick_time->tm_hour < 18 && tick_time->tm_hour >= 6) {
    layer_add_child(window_get_root_layer(s_main_window), inverter_layer_get_layer(s_invert_layer));
    daytime = true;
  }
  
  airplane_height = (rand() % 31) + 20;
  
  //update time
  static char time_text[] = "00:00";
  if(clock_is_24h_style() == true) {
    //use 24h time
     strftime(time_text, sizeof("00:00"), "%H:%M", tick_time);
  }
  else{
    //12h time
    strftime(time_text, sizeof("00:00"), "%I:%M", tick_time);
  }
  text_layer_set_text(s_time_layer, time_text);
}

static void update_airplane() {
  //get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  //update seconds
  int x = (2.58*tick_time->tm_sec) - 10;
  layer_set_frame(s_airplane_layer, GRect(x, airplane_height, 10, 3));
  
  if(tick_time->tm_sec==0) {
    update_time();
  }
}

static void tick_handler(struct tm *ticktime, TimeUnits units_changed) {
  update_airplane();
}

static void main_window_unload(Window *window) {
  // Destroy Window's child Layers here
  // Destroy City
  gbitmap_destroy(s_city_bitmap_black);
  gbitmap_destroy(s_city_bitmap_white);
  layer_destroy(s_background_layer);
  // Destroy Airplane
  gbitmap_destroy(s_airplane_bitmap);
  layer_destroy(s_airplane_layer);
  // Destroy Clouds
  gbitmap_destroy(s_clouds_bitmap);
  layer_destroy(s_clouds_layer);
  // Destroy Textlayers
  text_layer_destroy(s_time_layer);
  // Destroy Inverter Layer
  inverter_layer_destroy(s_invert_layer);
  // Destroy Font
  fonts_unload_custom_font(s_time_font);
}

static void init() {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_set_background_color(s_main_window, GColorBlack);
  window_stack_push(s_main_window, true);
  
  // Set airplane position
  update_time();
  update_airplane();
  
  //register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}