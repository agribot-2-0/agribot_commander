/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#pragma once
namespace agribot_behavior
{
enum class Condition
{
  Less,
  Equal,
  More,
  None
};

struct WeatherCondition
{
  Condition condition_;
  double value_;
};

class WeatherConditions
{
public:
  WeatherCondition temperature_, pressure_, wind_, humidity_;
  bool areWeatherConditionsFullfilled(double temp, double pres, double wind, double humi);
  bool isCurrentTemperatureFullfilled(double temp);
  bool isCurrentHumidityFullfilled(double humi);
  bool isCurrentAtmosphericPressureFullfilled(double pres);
  bool isCurrentWindSpeedFullfilled(double wind);
};
}  // namespace agribot_behavior
