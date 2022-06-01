#ifndef FORECAST
#define FORECAST

void syncDWDForecast();

char getForecastTodayIcon();
float getForecastTodayAvgTemp();
float getForecastTodayMaxTemp();
float getForecastTodayMinTemp();
float getForecastTodayRainAmt();
float getForecastTodayWindSpeed();
float getForecastTodaySunHrs();
float getForecastTodayOvercastPercent();

char getForecastTomorrowIcon();
float getForecastTomorrowAvgTemp();

char getForecastOvermomorrowIcon();
float getForecastOvermomorrowAvgTemp();

char getForecastOverOvermomorrowIcon();
float getForecastOverOvermomorrowAvgTemp();


#endif