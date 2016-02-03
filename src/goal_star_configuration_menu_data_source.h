#pragma once

#include "goal_star_configuration_option_menu_window.h"
#include "goal_star_number_window.h"

#include <pebble.h>

#include <stdint.h>

typedef int16_t (*GoalStarConfigurationMenuDataSourceOptionMultipleChoiceGetIndexOfCurrentChoiceCallback)(void);

typedef int32_t (*GoalStarConfigurationMenuDataSourceOptionNumberGetNumberCallback)(void);

typedef enum {
  GoalStarConfigurationMenuDataSourceOptionType_MultipleChoice,
  GoalStarConfigurationMenuDataSourceOptionType_Number,

  GoalStarConfigurationMenuDataSourceOptionType_Count
} GoalStarConfigurationMenuDataSourceOptionType;

typedef struct {
  const char *title;
  GoalStarConfigurationMenuDataSourceOptionType type;
  union {
    // GoalStarConfigurationMenuDataSourceOptionType_MultipleChoice
    struct {
      GoalStarConfigurationOptionMenuWindowCallbacks callbacks;
      GoalStarConfigurationMenuDataSourceOptionMultipleChoiceGetIndexOfCurrentChoiceCallback get_index_of_current_choice;
    } choice_callbacks;
    // GoalStarConfigurationMenuDataSourceOptionType_Number
    struct {
      GoalStarConfigurationMenuDataSourceOptionNumberGetNumberCallback get_lower_bound;
      GoalStarConfigurationMenuDataSourceOptionNumberGetNumberCallback get_upper_bound;
      GoalStarConfigurationMenuDataSourceOptionNumberGetNumberCallback get_current_value;
      // TODO replace with SDK NumberWindow once it gets updated to support larger numbers
      GoalStarNumberWindowCallback number_selected;
    } number_callbacks;
  };
} GoalStarConfigurationMenuDataSourceOption;

const GoalStarConfigurationMenuDataSourceOption *goal_star_configuration_menu_data_source_get_option_at_index(
  uint16_t index);
uint16_t goal_star_configuration_menu_data_source_get_num_options(void);
void goal_star_configuration_menu_data_source_get_current_choice_string_for_option_at_index(
  uint16_t index, char result[GOAL_STAR_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]);
