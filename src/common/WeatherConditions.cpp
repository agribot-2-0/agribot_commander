/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#include <agribot_behavior/common/WeatherConditions.h>
#include <ros/ros.h>
namespace agribot_behavior
{
bool WeatherConditions::areWeatherConditionsFullfilled(double current_temperature, double current_atmospheric_pressure,
                                                       double current_wind_speed, double current_humidity)
{
  return (isCurrentTemperatureFullfilled(current_temperature) && isCurrentHumidityFullfilled(current_humidity) &&
          isCurrentWindSpeedFullfilled(current_wind_speed) &&
          isCurrentAtmosphericPressureFullfilled(current_atmospheric_pressure));
}

bool WeatherConditions::isCurrentTemperatureFullfilled(double current_temperature)
{
  if (temperature_.condition_ == Condition::Less)
  {
    if (current_temperature < temperature_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << temperature_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_temperature;
      throw(std::logic_error(error_message_ + "Temperature should be less than " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C."));
    }
  }
  else if (temperature_.condition_ == Condition::Equal)
  {
    if (current_temperature == temperature_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << temperature_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_temperature;
      throw(std::logic_error(error_message_ + "Temperature should be equal to " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C."));
    }
  }
  else if (temperature_.condition_ == Condition::More)
  {
    if (current_temperature > temperature_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::fixed << std::setprecision(1) << temperature_.value_;
      std::stringstream value_current;
      value_current << std::fixed << std::setprecision(1) << current_temperature;
      throw(std::logic_error(error_message_ + "Temperature should be more than " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C."));
    }
  }

  return true;
}
bool WeatherConditions::isCurrentHumidityFullfilled(double current_humidity)
{
  if (humidity_.condition_ == Condition::Less)
  {
    if (current_humidity < humidity_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << humidity_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_humidity;
      throw(std::logic_error(error_message_ + "Humidity should be less than " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C."));
    }
  }
  else if (humidity_.condition_ == Condition::Equal)
  {
    if (current_humidity == humidity_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << humidity_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_humidity;
      throw(std::logic_error(error_message_ + "Humidity should be equal to " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C. "));
    }
  }
  else if (humidity_.condition_ == Condition::More)
  {
    if (current_humidity > humidity_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << humidity_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_humidity;
      throw(std::logic_error(error_message_ + "Humidity should be more than " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C. "));
    }
  }

  return true;
}
bool WeatherConditions::isCurrentAtmosphericPressureFullfilled(double current_atmospheric_pressure)
{
  if (atmospheric_pressure_.condition_ == Condition::Less)
  {
    if (current_atmospheric_pressure < atmospheric_pressure_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << atmospheric_pressure_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_atmospheric_pressure;
      throw(std::logic_error(error_message_ + "Atmospheric pressure should be less than " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C. "));
      return false;
    }
  }
  else if (atmospheric_pressure_.condition_ == Condition::Equal)
  {
    if (current_atmospheric_pressure == atmospheric_pressure_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << atmospheric_pressure_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_atmospheric_pressure;
      throw(std::logic_error(error_message_ + "Atmospheric pressure should be equal to " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C. "));
    }
  }
  else if (atmospheric_pressure_.condition_ == Condition::More)
  {
    if (current_atmospheric_pressure > atmospheric_pressure_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << atmospheric_pressure_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_atmospheric_pressure;
      throw(std::logic_error(error_message_ + "Atmspheric pressure should be more than " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C. "));
    }
  }

  return true;
}
bool WeatherConditions::isCurrentWindSpeedFullfilled(double current_wind_speed)
{
  if (wind_speed_.condition_ == Condition::Less)
  {
    if (current_wind_speed < wind_speed_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << wind_speed_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_wind_speed;
      throw(std::logic_error(error_message_ + "Wind speed should be less than " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C. "));
    }
  }
  else if (wind_speed_.condition_ == Condition::Equal)
  {
    if (current_wind_speed == wind_speed_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << wind_speed_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_wind_speed;
      throw(std::logic_error(error_message_ + "Wind speed should be equal to " + value_from_task.str() +
                             " deg C, but is " + value_current.str() + " deg C. "));
    }
  }
  else if (wind_speed_.condition_ == Condition::More)
  {
    if (current_wind_speed > wind_speed_.value_)
    {
      return true;
    }
    else
    {
      std::stringstream value_from_task;
      value_from_task << std::setprecision(1) << wind_speed_.value_;
      std::stringstream value_current;
      value_current << std::setprecision(1) << current_wind_speed;
      throw(std::logic_error(error_message_ + "Wind speed should be more than " + value_from_task.str() + ", but is " +
                             value_current.str() + ". "));
    }
  }

  return true;
}
std::string WeatherConditions::getMessage()
{
  ROS_ERROR_STREAM(error_message_);
  return error_message_;
}

}  // namespace agribot_behavior
