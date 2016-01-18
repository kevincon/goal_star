#include "goalie_configuration_menu_data_source.h"

#include "goalie_configuration.h"

#define MAKE_CHOICE_STRUCT(TYPE) \
typedef struct { \
  const char *choice_name; \
  TYPE choice_value; \
} TYPE ## Choice;

// Goal Type
/////////////

MAKE_CHOICE_STRUCT(HealthMetric)

static const HealthMetricChoice s_goal_type_options[] = {
  {
    .choice_name = "Steps",
    .choice_value = HealthMetricStepCount,
  },
  {
    .choice_name = "Distance",
    .choice_value = HealthMetricWalkedDistanceMeters,
  }
};

static uint16_t prv_goal_type_get_num_choices(void) {
  return ARRAY_LENGTH(s_goal_type_options);
}

static void prv_goal_type_get_string_for_index(
  uint16_t index, char result[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]) {
  if (!result) {
    return;
  }

  strncpy(result, s_goal_type_options[index].choice_name,
          GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH);
}

static void prv_goal_type_choice_made(uint16_t choice_index) {
  GoalieConfiguration *configuration = goalie_configuration_get_configuration();
  configuration->goal_type = s_goal_type_options[choice_index].choice_value;
}

static int16_t prv_goal_type_get_index_of_current_choice(void) {
  GoalieConfiguration *configuration = goalie_configuration_get_configuration();
  for (unsigned int choice_index = 0; choice_index < ARRAY_LENGTH(s_goal_type_options);
       choice_index++) {
    if (s_goal_type_options[choice_index].choice_value == configuration->goal_type) {
      return choice_index;
    }
  }
  return -1;
}

// Goal Value
/////////////

static int32_t prv_goal_value_get_lower_bound(void) {
  return 1;
}

static int32_t prv_goal_value_get_upper_bound(void) {
  return 100000;
}

static void prv_goal_value_number_selected(NumberWindow *number_window, void *context) {
  GoalieConfiguration *configuration = goalie_configuration_get_configuration();
  configuration->goal_value = number_window_get_value(number_window);

  const bool animated = true;
  window_stack_pop(animated);
}

static int32_t prv_goal_value_get_current_value(void) {
  GoalieConfiguration *configuration = goalie_configuration_get_configuration();
  return configuration->goal_value;
}

// Common
//////////

static const GoalieConfigurationMenuDataSourceOption s_options[] = {
  {
    .title = "Goal Type",
    .type = GoalieConfigurationMenuDataSourceOptionType_MultipleChoice,
    .choice_callbacks = {
      .get_index_of_current_choice = prv_goal_type_get_index_of_current_choice,
      .callbacks = {
        .get_string_for_index = prv_goal_type_get_string_for_index,
        .get_num_choices = prv_goal_type_get_num_choices,
        .choice_made = prv_goal_type_choice_made,
      },
    },
  },
  {
    .title = "Goal Value",
    .type = GoalieConfigurationMenuDataSourceOptionType_Number,
    .number_callbacks = {
      .get_lower_bound = prv_goal_value_get_lower_bound,
      .get_upper_bound = prv_goal_value_get_upper_bound,
      .number_selected = prv_goal_value_number_selected,
      .get_current_value = prv_goal_value_get_current_value,
    }
  }
};

const GoalieConfigurationMenuDataSourceOption *goalie_configuration_menu_data_source_get_option_at_index(
  uint16_t index) {
  return &s_options[index];
}

uint16_t goalie_configuration_menu_data_source_get_num_options(void) {
  return ARRAY_LENGTH(s_options);
}

void goalie_configuration_menu_data_source_get_current_choice_string_for_option_at_index(
  uint16_t index, char result[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]) {
  const GoalieConfigurationMenuDataSourceOption *option =
    goalie_configuration_menu_data_source_get_option_at_index(index);

  switch (option->type) {
    case GoalieConfigurationMenuDataSourceOptionType_MultipleChoice: {
      const int16_t current_choice_index =
        option->choice_callbacks.get_index_of_current_choice();
      if (current_choice_index != -1) {
        char current_choice_string[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH] =
          {0};
        option->choice_callbacks.callbacks.get_string_for_index(current_choice_index,
                                                                current_choice_string);
        strncpy(result, current_choice_string,
                GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH);
      }
      break;
    }
    case GoalieConfigurationMenuDataSourceOptionType_Number:
      // TODO
      break;
    default:
      return;
  }
}