#pragma once

#include "goalie_configuration_option_menu_window.h"

#include <pebble.h>

#include <stdint.h>

typedef int16_t (*GoalieConfigurationMenuDataSourceOptionMultipleChoiceGetIndexOfCurrentChoiceCallback)(void);

typedef int16_t (*GoalieConfigurationMenuDataSourceOptionNumberGetLowerBoundCallback)(void);
typedef int16_t (*GoalieConfigurationMenuDataSourceOptionNumberGetUpperBoundCallback)(void);

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
      GoalieConfigurationOptionMenuWindowCallbacks choice_callbacks;
      GoalieConfigurationMenuDataSourceOptionMultipleChoiceGetIndexOfCurrentChoiceCallback get_index_of_current_choice;
    };
    // GoalieConfigurationMenuDataSourceOptionType_Number
    struct {
      GoalieConfigurationMenuDataSourceOptionNumberGetLowerBoundCallback get_lower_bound;
      GoalieConfigurationMenuDataSourceOptionNumberGetUpperBoundCallback get_upper_bound;
      NumberWindowCallback number_selected;
    };
  };
} GoalieConfigurationMenuDataSourceOption;

const GoalieConfigurationMenuDataSourceOption *goalie_configuration_menu_data_source_get_option_at_index(
  uint16_t index);
uint16_t goalie_configuration_menu_data_source_get_num_options(void);
void goalie_configuration_menu_data_source_get_current_choice_string_for_option_at_index(
  uint16_t index, char result[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]);
