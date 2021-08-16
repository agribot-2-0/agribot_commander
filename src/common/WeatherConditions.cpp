/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#include <agribot_behavior/common/WeatherConditions.h>
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
    return (current_temperature < temperature_.value_);
  }
  else if (temperature_.condition_ == Condition::Equal)
  {
    return (current_temperature == temperature_.value_);
  }
  else if (temperature_.condition_ == Condition::More)
  {
    return (current_temperature > temperature_.value_);
  }
  return true;
}
bool WeatherConditions::isCurrentHumidityFullfilled(double current_humidity)
{
  if (humidity_.condition_ == Condition::Less)
  {
    return (current_humidity < humidity_.value_);
  }
  else if (humidity_.condition_ == Condition::Equal)
  {
    return (current_humidity == humidity_.value_);
  }
  else if (humidity_.condition_ == Condition::More)
  {
    return (current_humidity > humidity_.value_);
  }
  return true;
}
bool WeatherConditions::isCurrentAtmosphericPressureFullfilled(double current_atmospheric_pressure)
{
  if (pressure_.condition_ == Condition::Less)
  {
    return (current_atmospheric_pressure < pressure_.value_);
  }
  else if (pressure_.condition_ == Condition::Equal)
  {
    return (current_atmospheric_pressure == pressure_.value_);
  }
  else if (pressure_.condition_ == Condition::More)
  {
    return (current_atmospheric_pressure > pressure_.value_);
  }
  return true;
}
bool WeatherConditions::isCurrentWindSpeedFullfilled(double current_wind_speed)
{
  if (wind_.condition_ == Condition::Less)
  {
    return (current_wind_speed < wind_.value_);
  }
  else if (wind_.condition_ == Condition::Equal)
  {
    return (current_wind_speed == wind_.value_);
  }
  else if (wind_.condition_ == Condition::More)
  {
    return (current_wind_speed > wind_.value_);
  }
  return true;
}
}  // namespace agribot_behavior
