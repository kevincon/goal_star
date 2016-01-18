#pragma once

#include <stdint.h>

#define GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH 30

typedef uint16_t(*GoalieConfigurationOptionMenuWindowGetNumChoicesCallback)(void);
typedef void (*GoalieConfigurationOptionMenuWindowGetStringForIndexCallback)(
  uint16_t index, char result[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]);
typedef void (*GoalieConfigurationOptionMenuWindowChoiceMadeCallback)(uint16_t choice_index);

typedef struct {
  GoalieConfigurationOptionMenuWindowGetNumChoicesCallback get_num_choices;
  GoalieConfigurationOptionMenuWindowGetStringForIndexCallback get_string_for_index;
  GoalieConfigurationOptionMenuWindowChoiceMadeCallback choice_made;
} GoalieConfigurationOptionMenuWindowCallbacks;

typedef struct {
  const char *option_title;
  uint16_t current_choice;
  GoalieConfigurationOptionMenuWindowCallbacks callbacks;
} GoalieConfigurationOptionMenuWindowSettings;

void goalie_configuration_option_menu_window_push(
  const GoalieConfigurationOptionMenuWindowSettings *settings);
