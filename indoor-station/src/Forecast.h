#ifndef FORECAST
#define FORECAST

void syncDWDForecast();

float getForecastTodayIcon();
float getForecastTodayAvgTemp();
float getForecastTodayMaxTemp();
float getForecastTodayMinTemp();
float getForecastTodayRainAmt();
float getForecastTodayWindSpeed();
float getForecastTodaySunHrs();
float getForecastTodayOvercastPercent();

float getForecastTomorrowIcon();
float getForecastTomorrowAvgTemp();

float getForecastOvermomorrowIcon();
float getForecastOvermomorrowAvgTemp();

float getForecastOverOvermomorrowIcon();
float getForecastOverOvermomorrowAvgTemp();


#endif