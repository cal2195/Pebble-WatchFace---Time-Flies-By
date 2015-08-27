/*
 * main.c
 * Sets up a Window object and pushes it onto the stack.
 */

#include <pebble.h>

static Window *s_main_window;
static GBitmap *s_city_bitmap;
static BitmapLayer *s_bitmap_background_layer;

static void main_window_load(Window *window) {
  // Create Window's child Layers here
  s_city_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CITY_BACKGROUND);
  s_bitmap_background_layer = bitmap_layer_create(GRect(0, 20, 144, 104));
  bitmap_layer_set_bitmap(s_bitmap_background_layer, s_city_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bitmap_background_layer));
}

static void main_window_unload(Window *window) {
  // Destroy Window's child Layers here
  gbitmap_destroy(s_city_bitmap);
  bitmap_layer_destroy(s_bitmap_background_layer);
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