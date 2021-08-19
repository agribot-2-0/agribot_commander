/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#pragma once
#include <string>
#include <exception>
#include <iomanip>
namespace agribot_behavior
{
enum class Condition
{
  Less,
  Equal,
  More,
  None
};

enum class WeatherType
{
  Temperature,
  AtmosphericPressure,
  Humidity,
  WindSpeed
};

class WeatherCondition
{
private:
  inline static const std::string beginning_of_exception_message_ = "Abort task execution. ";

public:
  Condition condition_;
  double value_;
  WeatherType type_;
  std::string unit_;
  bool isFullfilled(double current_value);
  std::string getExceptionMessage(double from_task, double current, const std::string unit);
  std::string typeToString();
  std::string conditionToString();
};

class WeatherConditions
{
private:
  inline static const std::string temperature_unit_ = " deg C";
  inline static const std::string humidity_unit_ = " %";
  inline static const std::string wind_speed_unit_ = " m/s";
  inline static const std::string atmospheric_pressure_unit_ = " Pa";

public:
  WeatherConditions();
  WeatherCondition temperature_, atmospheric_pressure_, wind_speed_, humidity_;
  bool areWeatherConditionsFullfilled(double temp, double pres, double wind, double humi);
  bool isCurrentTemperatureFullfilled(double temp);
  bool isCurrentHumidityFullfilled(double humi);
  bool isCurrentAtmosphericPressureFullfilled(double pres);
  bool isCurrentWindSpeedFullfilled(double wind);
};
}  // namespace agribot_behavior
