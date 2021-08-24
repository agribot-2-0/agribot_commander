/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#include <agribot_behavior/common/WeatherConditions.h>
namespace agribot_behavior
{
std::string WeatherCondition::weatherTypeToString()
{
  switch (type_)
  {
    case WeatherType::Temperature:
      return "Temperature";

    case WeatherType::Humidity:
      return "Humidity";

    case WeatherType::AtmosphericPressure:
      return "Atmospheric pressure";

    case WeatherType::WindSpeed:
      return "Wind speed";
  }
  return "";
}

std::string WeatherCondition::conditionToString()
{
  switch (condition_)
  {
    case Condition::Less:
      return "less than ";

    case Condition::Equal:
      return "equal to ";

    case Condition::More:
      return "more than ";
    case Condition::None:
      return "";
  }
  return "";
}

bool WeatherCondition::isFullfilled(double current_value)
{
  switch (condition_)
  {
    case Condition::Less:
      if (current_value < value_)
      {
        return true;
      }
      else
      {
        throw(std::logic_error(getExceptionMessage(value_, current_value, unit_)));
      }

    case Condition::Equal:
      if (value_ == current_value)
      {
        return true;
      }
      else
      {
        throw(std::logic_error(getExceptionMessage(value_, current_value, unit_)));
      }

    case Condition::More:
      if (current_value > value_)
      {
        return true;
      }
      else
      {
        throw(std::logic_error(getExceptionMessage(value_, current_value, unit_)));
      }

    default:
      return true;
  }
}

WeatherConditions::WeatherConditions()
{
  temperature_.unit_ = temperature_unit_;
  humidity_.unit_ = humidity_unit_;
  wind_speed_.unit_ = wind_speed_unit_;
  atmospheric_pressure_.unit_ = atmospheric_pressure_unit_;
}

bool WeatherConditions::areWeatherConditionsFullfilled(double current_temperature, double current_atmospheric_pressure,
                                                       double current_wind_speed, double current_humidity)
{
  return (temperature_.isFullfilled(current_temperature) && humidity_.isFullfilled(current_humidity) &&
          wind_speed_.isFullfilled(current_wind_speed) &&
          atmospheric_pressure_.isFullfilled(current_atmospheric_pressure));
}

std::string WeatherCondition::getExceptionMessage(double from_task, double current, const std::string unit)
{
  std::stringstream value_from_task;
  value_from_task << std::fixed << std::setprecision(1) << from_task;
  std::stringstream value_current;
  value_current << std::fixed << std::setprecision(1) << current;
  return (beginning_of_exception_message_ + weatherTypeToString() + " should be " + conditionToString() +
          value_from_task.str() + unit + ", but is " + value_current.str() + unit + ".");
}
}  // namespace agribot_behavior
