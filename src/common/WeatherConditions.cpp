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
    case Condition::Undefined:
      return "undefined";
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
        writeExceptionMessage(value_, current_value, unit_);
        return false;
      }

    case Condition::Equal:
      if (value_ == current_value)
      {
        return true;
      }
      else
      {
        writeExceptionMessage(value_, current_value, unit_);
        return false;
      }

    case Condition::More:
      if (current_value > value_)
      {
        return true;
      }
      else
      {
        writeExceptionMessage(value_, current_value, unit_);
        return false;
      }

    case Condition::Undefined:
      writeExceptionMessage(value_, current_value, unit_);
      return false;

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
  std::string exception_message;

  if (!temperature_.isFullfilled(current_temperature))
  {
    exception_message += temperature_.getExceptionMessage();
  }
  if (!atmospheric_pressure_.isFullfilled(current_atmospheric_pressure))
  {
    exception_message += atmospheric_pressure_.getExceptionMessage();
  }
  if (!humidity_.isFullfilled(current_humidity))
  {
    exception_message += humidity_.getExceptionMessage();
  }
  if (!wind_speed_.isFullfilled(current_wind_speed))
  {
    exception_message += wind_speed_.getExceptionMessage();
  }

  if (exception_message.size() > 0)
  {
    throw(std::logic_error(beginning_of_exception_message_ + exception_message));
  }
  else
  {
    return true;
  }
}

void WeatherCondition::writeExceptionMessage(double from_task, double current, const std::string& unit)
{
  std::stringstream value_from_task;
  value_from_task << std::fixed << std::setprecision(1) << from_task;
  std::stringstream value_current;
  value_current << std::fixed << std::setprecision(1) << current;
  if (condition_ == Condition::Undefined)
  {
    exception_message = (weatherTypeToString() + " condition is " + conditionToString() + ". ");
  }
  else
  {
    exception_message = (weatherTypeToString() + " for this task should be " + conditionToString() +
                         value_from_task.str() + unit + ", but it is currently " + value_current.str() + unit + ". ");
  }
}
std::string WeatherCondition::getExceptionMessage()
{
  return exception_message;
}
}  // namespace agribot_behavior
