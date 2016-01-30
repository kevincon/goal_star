#include "goalie_configuration_window.h"

#include "goalie_configuration_menu_data_source.h"
#include "goalie_configuration_option_menu_window.h"
#include "goalie_number_window.h"

#include <pebble.h>

#include <inttypes.h>

typedef struct {
  Window *window;
  TextLayer *title_layer;
  MenuLayer *menu_layer;
  // TODO replace with SDK NumberWindow once it gets updated to support larger numbers
  GoalieNumberWindow *number_window;
} GoalieConfigurationWindowData;

static uint16_t prv_menu_layer_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index,
                                                     void *context) {
  return goalie_configuration_menu_data_source_get_num_options();
}

static void prv_menu_layer_draw_row_callback(GContext *ctx, const Layer *cell_layer,
                                             MenuIndex *cell_index, void *context) {
  const GoalieConfigurationMenuDataSourceOption *option =
    goalie_configuration_menu_data_source_get_option_at_index(cell_index->row);
  char subtitle[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH] = {0};
  switch (option->type) {
    case GoalieConfigurationMenuDataSourceOptionType_MultipleChoice:
      goalie_configuration_menu_data_source_get_current_choice_string_for_option_at_index(
        cell_index->row, subtitle);
      break;
    case GoalieConfigurationMenuDataSourceOptionType_Number:
      snprintf(subtitle, GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH, "%"PRId32"",
               option->number_callbacks.get_current_value());
      break;
    default:
      return;
  }
  menu_cell_basic_draw(ctx, cell_layer, option->title, subtitle, NULL);
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
  GoalieConfigurationWindowData *data = context;
  const GoalieConfigurationMenuDataSourceOption *option =
    goalie_configuration_menu_data_source_get_option_at_index(cell_index->row);

  switch (option->type) {
    case GoalieConfigurationMenuDataSourceOptionType_MultipleChoice: {
      const int16_t current_choice_index = option->choice_callbacks.get_index_of_current_choice();
      const GoalieConfigurationOptionMenuWindowSettings settings =
        (GoalieConfigurationOptionMenuWindowSettings) {
          .option_title = option->title,
          .callbacks = option->choice_callbacks.callbacks,
          .current_choice = (current_choice_index == -1) ? (uint16_t)0 :
                                                           (uint16_t)current_choice_index,
        };
      goalie_configuration_option_menu_window_push(&settings);
      break;
    }
    case GoalieConfigurationMenuDataSourceOptionType_Number: {
      // TODO replace with SDK NumberWindow once it gets updated to support larger numbers
      goalie_number_window_destroy(data->number_window);
      data->number_window = goalie_number_window_create(option->title,
                                                        (GoalieNumberWindowCallbacks) {
        .selected = option->number_callbacks.number_selected,
      }, data);
      GoalieNumberWindow *number_window = data->number_window;
      goalie_number_window_set_value(number_window, option->number_callbacks.get_current_value());
      goalie_number_window_set_min(number_window, option->number_callbacks.get_lower_bound());
      goalie_number_window_set_max(number_window, option->number_callbacks.get_upper_bound());
      goalie_number_window_set_step_size(number_window, 100);
      const bool animated = true;
      window_stack_push(goalie_number_window_get_window(number_window), animated);
      break;
    }
    default:
      return;
  }
}

static void prv_window_load(Window *window) {
  GoalieConfigurationWindowData *data = window_get_user_data(window);
  if (!data) {
    return;
  }

  Layer *window_root_layer = window_get_root_layer(window);
  const GRect window_root_layer_bounds = layer_get_bounds(window_root_layer);

  const GRect title_layer_frame = (GRect) {
#if PBL_RECT
    // Adjust for font cap offset
    .origin = GPoint(0, -2),
#endif
    .size = GSize(window_root_layer_bounds.size.w, STATUS_BAR_LAYER_HEIGHT),
  };
  data->title_layer = text_layer_create(title_layer_frame);
  TextLayer *title_layer = data->title_layer;
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(title_layer, GTextOverflowModeTrailingEllipsis);
  text_layer_set_background_color(title_layer, GColorClear);
  text_layer_set_text_color(title_layer, GColorBlack);
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(title_layer, PBL_IF_RECT_ELSE("Goalie Configuration", "Config"));
  layer_add_child(window_root_layer, text_layer_get_layer(title_layer));

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
  layer_add_child(window_root_layer, menu_layer_get_layer(menu_layer));
}

static void prv_window_unload(Window *window) {
  GoalieConfigurationWindowData *data = window_get_user_data(window);

  if (data) {
    // TODO replace with SDK NumberWindow once it gets updated to support larger numbers
    goalie_number_window_destroy(data->number_window);
    menu_layer_destroy(data->menu_layer);
    text_layer_destroy(data->title_layer);
    window_destroy(data->window);
  }

  free(data);
}

void goalie_configuration_window_push(void) {
  GoalieConfigurationWindowData *data = calloc(1, sizeof(*data));
  if (!data) {
    return;
  }

  data->window = window_create();
  Window *window = data->window;

  window_set_window_handlers(window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  window_set_user_data(window, data);

  // The window push animation on rectangular displays looks a little weird with the ring
  const bool animated = PBL_IF_RECT_ELSE(false, true);
  window_stack_push(window, animated);
}
