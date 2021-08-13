/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#include <agribot_behavior/common/WeatherConditions.h>
namespace agribot_behavior
{
bool WeatherConditions::areWeatherConditionsOkay(double temp, double pres, double wind, double humi)
{
  return (compareTemperature(temp) && compareHumidity(humi) && compareWind(wind) && comparePressure(pres));
}

bool WeatherConditions::compareTemperature(double temp)
{
  if (temperature_.condition_ == Condition::less)
  {
    if (temp < temperature_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (temperature_.condition_ == Condition::equal)
  {
    if (temp == temperature_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (temperature_.condition_ == Condition::more)
  {
    if (temp > temperature_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return true;
}
bool WeatherConditions::compareHumidity(double humi)
{
  if (humidity_.condition_ == Condition::less)
  {
    if (humi < humidity_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (humidity_.condition_ == Condition::equal)
  {
    if (humi == humidity_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (humidity_.condition_ == Condition::more)
  {
    if (humi > humidity_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return true;
}
bool WeatherConditions::comparePressure(double pres)
{
  if (pressure_.condition_ == Condition::less)
  {
    if (pres < pressure_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (pressure_.condition_ == Condition::equal)
  {
    if (pres == pressure_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (pressure_.condition_ == Condition::more)
  {
    if (pres > pressure_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return true;
}
bool WeatherConditions::compareWind(double wind)
{
  if (wind_.condition_ == Condition::less)
  {
    if (wind < wind_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (wind_.condition_ == Condition::equal)
  {
    if (wind == wind_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if (wind_.condition_ == Condition::more)
  {
    if (wind > wind_.value_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return true;
}
}  // namespace agribot_behavior
