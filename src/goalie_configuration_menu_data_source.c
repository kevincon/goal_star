#include "goalie_configuration_menu_data_source.h"

#include "../common/goalie_configuration.h"

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
  goalie_configuration_set_goal_type(s_goal_type_options[choice_index].choice_value);
}

static int16_t prv_goal_type_get_index_of_current_choice(void) {
  const HealthMetric current_goal_type = goalie_configuration_get_goal_type();
  for (unsigned int choice_index = 0; choice_index < ARRAY_LENGTH(s_goal_type_options);
       choice_index++) {
    if (s_goal_type_options[choice_index].choice_value == current_goal_type) {
      return (int16_t)choice_index;
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

static void prv_goal_value_number_selected(GoalieNumberWindow *number_window, void *context) {
  // TODO replace with SDK NumberWindow once it gets updated to support larger numbers
  goalie_configuration_set_goal_value(goalie_number_window_get_value(number_window));

  const bool animated = true;
  window_stack_pop(animated);
}

static int32_t prv_goal_value_get_current_value(void) {
  return goalie_configuration_get_goal_value();
}

// Goal Event Window Timeout
//////////////////////////////

typedef uint32_t GoalEventTimeoutMsValue;

MAKE_CHOICE_STRUCT(GoalEventTimeoutMsValue)

static const GoalEventTimeoutMsValueChoice s_goal_event_timeout_ms_options[] = {
  {
    .choice_name = "Stay on screen",
    .choice_value = 0,
  },
  {
    .choice_name = "1 second",
    .choice_value = 1000,
  },
  {
    .choice_name = "3 seconds",
    .choice_value = 3000,
  },
  {
    .choice_name = "5 seconds",
    .choice_value = 5000,
  },
  {
    .choice_name = "10 seconds",
    .choice_value = 10000,
  },
  {
    .choice_name = "30 seconds",
    .choice_value = 30000,
  },
  {
    .choice_name = "1 minute",
    .choice_value = 60000,
  },
  {
    .choice_name = "3 minutes",
    .choice_value = 180000,
  },
  {
    .choice_name = "5 minutes",
    .choice_value = 300000,
  },
  {
    .choice_name = "10 minutes",
    .choice_value = 600000,
  }
};

static uint16_t prv_goal_event_timeout_ms_get_num_choices(void) {
  return ARRAY_LENGTH(s_goal_event_timeout_ms_options);
}

static void prv_goal_event_timeout_ms_get_string_for_index(
  uint16_t index, char result[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]) {
  if (!result) {
    return;
  }

  strncpy(result, s_goal_event_timeout_ms_options[index].choice_name,
          GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH);
}

static void prv_goal_event_timeout_ms_choice_made(uint16_t choice_index) {
  goalie_configuration_set_goal_event_timeout_ms(
    s_goal_event_timeout_ms_options[choice_index].choice_value);
}

static int16_t prv_goal_event_timeout_ms_get_index_of_current_choice(void) {
  const uint32_t current_goal_event_timeout_ms = goalie_configuration_get_goal_event_timeout_ms();
  for (unsigned int choice_index = 0; choice_index < ARRAY_LENGTH(s_goal_event_timeout_ms_options);
       choice_index++) {
    if (s_goal_event_timeout_ms_options[choice_index].choice_value ==
        current_goal_event_timeout_ms) {
      return (int16_t)choice_index;
    }
  }
  return -1;
}

// Boolean
////////////

#define ON_INDEX 0
#define OFF_INDEX 1

static uint16_t prv_bool_get_num_choices(void) {
  return 2;
}

static void prv_bool_get_string_for_index(
  uint16_t index, char result[GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH]) {
  if (!result) {
    return;
  }

  strncpy(result, (index == ON_INDEX) ? "Enabled" : "Disabled",
          GOALIE_CONFIGURATION_OPTION_MENU_WINDOW_CHOICE_BUFFER_LENGTH);
}

// Show clock time
///////////////////

static void prv_clock_time_enabled_choice_made(uint16_t choice_index) {
  goalie_configuration_set_clock_time_enabled((choice_index == ON_INDEX));
}

static int16_t prv_clock_time_enabled_get_index_of_current_choice(void) {
  const bool current_choice = goalie_configuration_get_clock_time_enabled();
  return (int16_t)(current_choice ? ON_INDEX : OFF_INDEX);
}

// Common
//////////

static const GoalieConfigurationMenuDataSourceOption s_options[] = {
  {
    .title = "Goal type",
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
    .title = "Goal value",
    .type = GoalieConfigurationMenuDataSourceOptionType_Number,
    .number_callbacks = {
      .get_lower_bound = prv_goal_value_get_lower_bound,
      .get_upper_bound = prv_goal_value_get_upper_bound,
      .number_selected = prv_goal_value_number_selected,
      .get_current_value = prv_goal_value_get_current_value,
    }
  },
  {
    .title = "Popup timeout",
    .type = GoalieConfigurationMenuDataSourceOptionType_MultipleChoice,
    .choice_callbacks = {
      .get_index_of_current_choice = prv_goal_event_timeout_ms_get_index_of_current_choice,
      .callbacks = {
        .get_string_for_index = prv_goal_event_timeout_ms_get_string_for_index,
        .get_num_choices = prv_goal_event_timeout_ms_get_num_choices,
        .choice_made = prv_goal_event_timeout_ms_choice_made,
      },
    },
  },
  {
    .title = "Show time",
    .type = GoalieConfigurationMenuDataSourceOptionType_MultipleChoice,
    .choice_callbacks = {
      .get_index_of_current_choice = prv_clock_time_enabled_get_index_of_current_choice,
      .callbacks = {
        .get_string_for_index = prv_bool_get_string_for_index,
        .get_num_choices = prv_bool_get_num_choices,
        .choice_made = prv_clock_time_enabled_choice_made,
      },
    },
  },
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
        option->choice_callbacks.callbacks.get_string_for_index((uint16_t)current_choice_index,
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
