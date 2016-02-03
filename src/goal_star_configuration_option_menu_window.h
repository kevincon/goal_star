#pragma once

#include <stdint.h>

#define GOAL_STAR_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH 30

typedef uint16_t(*GoalStarConfigurationOptionMenuWindowGetNumChoicesCallback)(void);
typedef void (*GoalStarConfigurationOptionMenuWindowGetStringForIndexCallback)(
  uint16_t index, char result[GOAL_STAR_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]);
typedef void (*GoalStarConfigurationOptionMenuWindowChoiceMadeCallback)(uint16_t choice_index);

typedef struct {
  GoalStarConfigurationOptionMenuWindowGetNumChoicesCallback get_num_choices;
  GoalStarConfigurationOptionMenuWindowGetStringForIndexCallback get_string_for_index;
  GoalStarConfigurationOptionMenuWindowChoiceMadeCallback choice_made;
} GoalStarConfigurationOptionMenuWindowCallbacks;

typedef struct {
  const char *option_title;
  uint16_t current_choice;
  GoalStarConfigurationOptionMenuWindowCallbacks callbacks;
} GoalStarConfigurationOptionMenuWindowSettings;

void goal_star_configuration_option_menu_window_push(
  const GoalStarConfigurationOptionMenuWindowSettings *settings);
