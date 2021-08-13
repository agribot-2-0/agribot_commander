/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

namespace agribot_behavior
{
enum class Condition
{
  less,
  equal,
  more,
  none
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
  bool areWeatherConditionsOkay(double temp, double pres, double wind, double humi);
  bool compareTemperature(double temp);
  bool compareHumidity(double humi);
  bool comparePressure(double pres);
  bool compareWind(double wind);
};
}  // namespace agribot_behavior