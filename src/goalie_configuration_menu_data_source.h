#pragma once

#include "goalie_configuration_option_menu_window.h"
#include "goalie_number_window.h"

#include <pebble.h>

#include <stdint.h>

typedef int16_t (*GoalieConfigurationMenuDataSourceOptionMultipleChoiceGetIndexOfCurrentChoiceCallback)(void);

typedef int32_t (*GoalieConfigurationMenuDataSourceOptionNumberGetNumberCallback)(void);

typedef enum {
  GoalieConfigurationMenuDataSourceOptionType_MultipleChoice,
  GoalieConfigurationMenuDataSourceOptionType_Number,

  GoalieConfigurationMenuDataSourceOptionType_Count
} GoalieConfigurationMenuDataSourceOptionType;

typedef struct {
  const char *title;
  GoalieConfigurationMenuDataSourceOptionType type;
  union {
    // GoalieConfigurationMenuDataSourceOptionType_MultipleChoice
    struct {
      GoalieConfigurationOptionMenuWindowCallbacks callbacks;
      GoalieConfigurationMenuDataSourceOptionMultipleChoiceGetIndexOfCurrentChoiceCallback get_index_of_current_choice;
    } choice_callbacks;
    // GoalieConfigurationMenuDataSourceOptionType_Number
    struct {
      GoalieConfigurationMenuDataSourceOptionNumberGetNumberCallback get_lower_bound;
      GoalieConfigurationMenuDataSourceOptionNumberGetNumberCallback get_upper_bound;
      GoalieConfigurationMenuDataSourceOptionNumberGetNumberCallback get_current_value;
      // TODO replace with SDK NumberWindow once it gets updated to support larger numbers
      GoalieNumberWindowCallback number_selected;
    } number_callbacks;
  };
} GoalieConfigurationMenuDataSourceOption;

const GoalieConfigurationMenuDataSourceOption *goalie_configuration_menu_data_source_get_option_at_index(
  uint16_t index);
uint16_t goalie_configuration_menu_data_source_get_num_options(void);
void goalie_configuration_menu_data_source_get_current_choice_string_for_option_at_index(
  uint16_t index, char result[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]);
