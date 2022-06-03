#ifndef FORECAST
#define FORECAST

void syncDWDForecast();

char getForecastTodayIcon();
float getForecastTodayAvgTemp();
float getForecastTodayMaxTemp();
float getForecastTodayMinTemp();
float getForecastTodayRainAmt();
float getForecastTodayWindSpeed();
const char* getForecastTodaySunHrs();
float getForecastTodayOvercastPercent();

char getForecastTomorrowIcon();
float getForecastTomorrowAvgTemp();

char getForecastOvermorrowIcon();
float getForecastOvermorrowAvgTemp();

char getForecastOverOvermorrowIcon();
float getForecastOverOvermorrowAvgTemp();


#endif