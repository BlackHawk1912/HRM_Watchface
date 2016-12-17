#include <pebble.h>
#include "main.h"

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer; //Layer layer for current date
static BitmapLayer *s_background_layer; //make an (background)image layer
static GBitmap *s_background_bitmap; //image for the (background) layer

//Bluetooth
static BitmapLayer *s_bt_icon_layer; // Bluetooth Icon layer
static GBitmap *s_bt_icon_bitmap; //image for the (background) layer

//Batterie
static Layer *s_battery_layer;
static int s_battery_level;

//Weather
static TextLayer *s_weather_layer;
static GFont s_weather_font;

//connection layer
static TextLayer *s_connection_layer;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];

  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);

  // If all data is available, use it
  if(temp_tuple && conditions_tuple) {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);

    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
  }
}

//Layers und Bitmaps for the spikes
//Hour First Digit
static BitmapLayer *s_h1_layer; 
static GBitmap *s_h1_bitmap;
//Hour Second Digit
static BitmapLayer *s_h2_layer; 
static GBitmap *s_h2_bitmap;
//Minute First Digit
static BitmapLayer *s_m1_layer; 
static GBitmap *s_m1_bitmap;
//Minute Second Digit
static BitmapLayer *s_m2_layer; 
static GBitmap *s_m2_bitmap;
static char s_buffer[8];

bool show_date=true;
//======================================================================================
static void update_time() {
	// Get a tm structure
	time_t temp = time(NULL); 
	struct tm *tick_time = localtime(&temp);

	// Write the current hours and minutes into a buffer
	strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?	"%H:%M" : "%I:%M", tick_time);
	// Display this time on the TextLayer
	text_layer_set_text(s_time_layer, s_buffer);

	//--- DATE CODE---
	// Need to be static because they're used by the system later.
	static char s_date_text[] = "Xxxx 00";

	strftime(s_date_text, sizeof(s_date_text), "%d %b", tick_time);

	text_layer_set_text(s_date_layer, s_date_text);  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	
	s_h1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);
	s_h1_layer = bitmap_layer_create(GRect(-45, -17, 144, 168)); //set the size and poz of the bg layer
	bitmap_layer_set_bitmap(s_h1_layer, s_h1_bitmap); //set the image to the layer
	layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_h1_layer));
	
	s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);
	s_h2_layer = bitmap_layer_create(GRect(-15, -17, 144, 168)); //set the size and poz of the bg layer
	bitmap_layer_set_bitmap(s_h2_layer, s_h2_bitmap); //set the image to the layer
	layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_h2_layer));

	s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);
	s_m1_layer = bitmap_layer_create(GRect(15, -17, 144, 168)); //set the size and poz of the bg layer
	bitmap_layer_set_bitmap(s_m1_layer, s_m1_bitmap); //set the image to the layer
	layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_m1_layer));

	s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);
	s_m2_layer = bitmap_layer_create(GRect(45, -17, 144, 168)); //set the size and poz of the bg layer
	bitmap_layer_set_bitmap(s_m2_layer, s_m2_bitmap); //set the image to the layer
	layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_m2_layer));
	
	/*below are the images for the peakes set ===================================================================================================*/
	int hourDigit1 = (int)s_buffer[0] - 48;
	int hourDigit2 = (int)s_buffer[1] - 48;
	//int seperator1 = s_buffer[2];
	int minDigit1 = (int)s_buffer[3] - 48;
	int minDigit2 = (int)s_buffer[4] - 48;
	
	//Hour first Digit	
	switch (hourDigit1) {
		case 0: 
			gbitmap_destroy(s_h1_bitmap);
			s_h1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);break;
		case 1: 
			gbitmap_destroy(s_h1_bitmap);
			s_h1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_1);break;
		case 2:
			gbitmap_destroy(s_h1_bitmap);
			s_h1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_2);break;
		default:
			gbitmap_destroy(s_h1_bitmap);
			s_h1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);break;
	}
	//Hour Second Digit
	switch (hourDigit2) {
		case 0:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);break;
		case 1:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_1);break;
		case 2:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_2);break;
		case 3:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_3);break;
		case 4:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_4);break;
		case 5:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_5);break;
		case 6:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_6);break;
		case 7:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_7);break;
		case 8:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_8);break;
		case 9:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_9);break;
		default:
			gbitmap_destroy(s_h2_bitmap);
			s_h2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);break;
	}
	//Minute first Digit
	switch (minDigit1) {
		case 0:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);break;
		case 1:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_1);break;
		case 2:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_2);break;
		case 3:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_3);break;
		case 4:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_4);break;
		case 5:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_5);break;
		case 6:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_6);break;
		case 7:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_7);break;
		case 8:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_8);break;
		case 9:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_9);break;
		default:
			gbitmap_destroy(s_m1_bitmap);
			s_m1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);break;
	}
	//Minute second Digit
	switch (minDigit2) {
		case 0:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);break;
		case 1:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_1);break;
		case 2:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_2);break;
		case 3:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_3);break;
		case 4:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_4);break;
		case 5:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_5);break;
		case 6:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_6);break;
		case 7:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_7);break;
		case 8:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_8);break;
		case 9:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_9);break;
		default:
			gbitmap_destroy(s_m2_bitmap);
			s_m2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PEAK_0);break;
	}
	/*above are the images for the peakes set ===================================================================================================*/
}

/*static void bluetooth_callback(bool connected) {
	// Show icon if disconnected
	layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);

	if(!connected) {
		// Issue a vibrating alert
		vibes_double_pulse();
	}
}*/
static void handle_bluetooth(bool connected) {
	//text_layer_set_text(s_connection_layer, (connected ? "connected" : "disconnected"));
}
static void battery_callback(BatteryChargeState state) {
	// Record the battery level
	s_battery_level = state.charge_percent;
	// Update meter
	layer_mark_dirty(s_battery_layer);
}
static void battery_update_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);

	// Find the width of the bar (total width = 144px)
	int width = (s_battery_level * 144) / 100;

	// Draw the background
	graphics_context_set_fill_color(ctx, GColorDarkGreen);
	graphics_fill_rect(ctx, bounds, 0, GCornerNone);

	// Draw the bar
	graphics_context_set_fill_color(ctx, GColorMediumAquamarine);
	graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void main_window_load(Window *window) {
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	GRect window_bounds = layer_get_bounds(window_layer);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_time_layer = text_layer_create(
		GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

	// Improve the layout to be more like a watchface
	text_layer_set_text(s_time_layer, "00:00");
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	// Create GBitmap, then set to created BitmapLayer  (Backgound HAS TO BE BEFORE text)
	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_hrm_bg);  //RESOURCE_ID_"Name of image file"
	s_background_layer = bitmap_layer_create(window_bounds);
	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap); //set the background to the image layer
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
	
	//Create Date layer:
	s_date_layer = text_layer_create(GRect(0, 121, 144, 47));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, GColorMediumAquamarine); //blueish text

	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	if (show_date == true) {
		layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
	}

	// Create the Bluetooth icon GBitmap
	/*s_bt_icon_layer = bitmap_layer_create(GRect(50, 50, 50, 50));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_icon_layer));
	s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);*/
	// Create the BitmapLayer to display the GBitmap
	bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
	//bluetooth_callback(connection_service_peek_pebble_app_connection());

	// Create battery meter Layer
	s_battery_layer = layer_create(GRect(0, 166, 144, 2));
	layer_set_update_proc(s_battery_layer, battery_update_proc);
	// Add to Window
	layer_add_child(window_get_root_layer(window), s_battery_layer);
	// Ensure battery level is displayed from the start
	battery_callback(battery_state_service_peek());

	// Create temperature Layer
	s_weather_layer = text_layer_create(
	GRect(0, PBL_IF_ROUND_ELSE(125, 120), bounds.size.w, 25));
	// Style the text
	text_layer_set_background_color(s_weather_layer, GColorClear);
	text_layer_set_text_color(s_weather_layer, GColorWhite);
	text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
	//text_layer_set_text(s_weather_layer, "Loading...");
	text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
	
	//connection layer
	s_connection_layer = text_layer_create(GRect(0, 90, bounds.size.w, 34));
	text_layer_set_text_color(s_connection_layer, GColorWhite);
	text_layer_set_background_color(s_connection_layer, GColorClear);
	text_layer_set_font(s_connection_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
	handle_bluetooth(bluetooth_connection_service_peek());
	layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));
}

static void main_window_unload(Window *window) {
	// Destroy TextLayer
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);

	bitmap_layer_destroy(s_bt_icon_layer);
	layer_destroy(s_battery_layer);
	// Destroy weather elements
	text_layer_destroy(s_weather_layer);
	fonts_unload_custom_font(s_weather_font);
	
	//destroy connection layer
	text_layer_destroy(s_connection_layer);
}

static void init() {
	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	// Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);

	// Make sure the time is displayed from the start
	update_time();

	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	// Register for battery level updates
	battery_state_service_subscribe(battery_callback);
	
	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	// Open AppMessage
	const int inbox_size = 128;
	const int outbox_size = 128;
	app_message_open(inbox_size, outbox_size);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}