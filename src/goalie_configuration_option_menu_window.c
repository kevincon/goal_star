#include "goalie_configuration_option_menu_window.h"

#include <pebble.h>

typedef struct {
  Window *window;
#if PBL_RECT
  TextLayer *title_layer;
#endif
  MenuLayer *menu_layer;
  GoalieConfigurationOptionMenuWindowSettings settings;
} GoalieConfigurationOptionMenuWindowData;

static uint16_t prv_menu_layer_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index,
                                                     void *context) {
  GoalieConfigurationOptionMenuWindowData *data = context;
  return data->settings.callbacks.get_num_choices();
}

static void prv_menu_layer_draw_row_callback(GContext *ctx, const Layer *cell_layer,
                                             MenuIndex *cell_index, void *context) {
  const bool row_is_highlighted = menu_cell_layer_is_highlighted(cell_layer);

  GoalieConfigurationOptionMenuWindowData *data = context;
  char title[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH] = {0};
  data->settings.callbacks.get_string_for_index(cell_index->row, title);

  const GRect cell_layer_bounds = layer_get_bounds(cell_layer);

  const int16_t radio_button_horizontal_padding = PBL_IF_RECT_ELSE(6, 10);
  const int16_t radio_button_radius = 7;

  int16_t right_inset = (radio_button_horizontal_padding + radio_button_radius) * 2;
  int16_t left_inset = 5;
#if PBL_ROUND
  right_inset += 25;
#endif

  const GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  const int16_t font_height = 24;
  const int16_t font_cap_height = 5;
  const int16_t vertical_inset = (cell_layer_bounds.size.h - font_height) / 2;
  GRect inset_cell_layer_bounds = grect_inset(cell_layer_bounds,
                                              GEdgeInsets(vertical_inset, right_inset,
                                                          vertical_inset, left_inset));
  inset_cell_layer_bounds.origin.y -= font_cap_height;
  const GTextAlignment text_alignment = PBL_IF_RECT_ELSE(GTextAlignmentLeft, GTextAlignmentRight);
  graphics_draw_text(ctx, title, font, inset_cell_layer_bounds, GTextOverflowModeTrailingEllipsis,
                     text_alignment, NULL);

  const GRect radio_button_container_frame = grect_inset(
    cell_layer_bounds,
    GEdgeInsets(0, 0, 0,
                inset_cell_layer_bounds.origin.x + inset_cell_layer_bounds.size.w + radio_button_horizontal_padding));
  GRect radio_button_frame = (GRect) { .size = GSize(radio_button_radius * 2,
                                                     radio_button_radius * 2) };
  const GAlign radio_button_alignment = PBL_IF_RECT_ELSE(GAlignCenter, GAlignLeft);
  grect_align(&radio_button_frame, &radio_button_container_frame, radio_button_alignment,
              true /* clip */);

  graphics_context_set_fill_color(ctx, row_is_highlighted ? GColorWhite : GColorBlack);
  const int16_t radio_button_outer_ring_thickness = 2;
  graphics_fill_radial(ctx, radio_button_frame, GOvalScaleModeFitCircle,
                       radio_button_outer_ring_thickness, 0, TRIG_MAX_ANGLE);

  if (cell_index->row == data->settings.current_choice) {
    radio_button_frame = grect_crop(radio_button_frame, radio_button_outer_ring_thickness + 2);
    graphics_fill_radial(ctx, radio_button_frame, GOvalScaleModeFitCircle,
                         radio_button_radius - radio_button_outer_ring_thickness - 1, 0,
                         TRIG_MAX_ANGLE);
  }
}

#if PBL_ROUND
static int16_t prv_menu_layer_get_cell_height(MenuLayer *menu_layer, MenuIndex *cell_index,
                                              void *data) {
  if (menu_layer_is_index_selected(menu_layer, cell_index)) {
    return MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT;
  } else {
    return MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT;
  }
}
#endif

static void prv_menu_layer_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index,
                                           void *context) {
  GoalieConfigurationOptionMenuWindowData *data = context;
  if (data->settings.callbacks.choice_made) {
    data->settings.callbacks.choice_made(cell_index->row);
  }
  const bool animated = true;
  window_stack_pop(animated);
}

static void prv_window_load(Window *window) {
  GoalieConfigurationOptionMenuWindowData *data = window_get_user_data(window);
  if (!data) {
    return;
  }

  Layer *window_root_layer = window_get_root_layer(window);
  const GRect window_root_layer_bounds = layer_get_bounds(window_root_layer);
#if PBL_RECT
  const GRect title_layer_frame = (GRect) {
    // Adjust for font cap offset
    .origin = GPoint(0, -2),
    .size = GSize(window_root_layer_bounds.size.w, STATUS_BAR_LAYER_HEIGHT),
  };
  data->title_layer = text_layer_create(title_layer_frame);
  TextLayer *title_layer = data->title_layer;
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(title_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_background_color(title_layer, GColorClear);
  text_layer_set_text_color(title_layer, GColorBlack);
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(title_layer, data->settings.option_title);
  layer_add_child(window_root_layer, text_layer_get_layer(title_layer));
#endif

  const GEdgeInsets menu_layer_insets = PBL_IF_RECT_ELSE(GEdgeInsets(STATUS_BAR_LAYER_HEIGHT, 0, 0),
                                                         GEdgeInsets(STATUS_BAR_LAYER_HEIGHT, 0));
  const GRect menu_layer_frame = grect_inset(window_root_layer_bounds, menu_layer_insets);
  data->menu_layer = menu_layer_create(menu_layer_frame);
  MenuLayer *menu_layer = data->menu_layer;
  menu_layer_set_callbacks(menu_layer, data, (MenuLayerCallbacks) {
    .get_num_rows = prv_menu_layer_get_num_rows_callback,
    .draw_row = prv_menu_layer_draw_row_callback,
#if PBL_ROUND
    .get_cell_height = prv_menu_layer_get_cell_height,
#endif
    .select_click = prv_menu_layer_select_callback,
  });
  menu_layer_set_normal_colors(menu_layer, GColorWhite, GColorBlack);
  menu_layer_set_highlight_colors(menu_layer, GColorCobaltBlue, GColorWhite);
  menu_layer_set_click_config_onto_window(menu_layer, window);
  menu_layer_set_selected_index(menu_layer, MenuIndex(0, data->settings.current_choice),
                                MenuRowAlignCenter, false /* animated */);
  layer_add_child(window_root_layer, menu_layer_get_layer(menu_layer));
}

static void prv_window_unload(Window *window) {
  GoalieConfigurationOptionMenuWindowData *data = window_get_user_data(window);

  if (data) {
    menu_layer_destroy(data->menu_layer);
#if PBL_RECT
    text_layer_destroy(data->title_layer);
#endif
    window_destroy(data->window);
  }

  free(data);
}

void goalie_configuration_option_menu_window_push(
  const GoalieConfigurationOptionMenuWindowSettings *settings) {
  if (!settings) {
    return;
  }

  GoalieConfigurationOptionMenuWindowData *data = calloc(1, sizeof(*data));
  if (!data) {
    return;
  }

  data->settings = *settings;

  data->window = window_create();
  Window *window = data->window;

  window_set_window_handlers(window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_set_user_data(window, data);

  const bool animated = true;
  window_stack_push(window, animated);
}
