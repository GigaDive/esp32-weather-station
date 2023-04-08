#ifndef FORECAST
#define FORECAST

void syncDWDForecast();

char getForecastTodayIcon();
float getForecastTodayAvgTemp();
float getForecastTodayMaxTemp();
float getForecastTodayMinTemp();
float getForecastTodayRainAmt();
float getForecastTodayWindSpeed();
int getForecastTodaySunHrs(); 
int getForecastTodaySunMin(); 
int getForecastTodayOvercastPercent();

char getForecastTomorrowIcon();
float getForecastTomorrowTemp();

char getForecastOvermorrowIcon();
float getForecastOvermorrowTemp();

char getForecastOverOvermorrowIcon();
float getForecastOverOvermorrowTemp();


#endif