using namespace std;

#include "services/IconMapper.h"
#include <string.h>

const char iconTypeToMeteocon(const char *iconString)
{
    if (strcmp(iconString, "clear-day") == 0)
    {
        return 'B';
    }
    else if (strcmp(iconString, "clear-night") == 0)
    {
        return 'C';
    }
    else if (strcmp(iconString, "partly-cloudy-day") == 0)
    {
        return 'H';
    }
    else if (strcmp(iconString, "partly-cloudy-night") == 0)
    {
        return 'I';
    }
    else if (strcmp(iconString, "cloudy") == 0)
    {
        return 'Y';
    }
    else if (strcmp(iconString, "fog") == 0)
    {
        return 'M';
    }
    else if (strcmp(iconString, "wind") == 0)
    {
        return 'F';
    }
    else if (strcmp(iconString, "rain") == 0)
    {
        return 'R';
    }
    else if (strcmp(iconString, "sleet") == 0)
    {
        return 'V';
    }
    else if (strcmp(iconString, "snow") == 0)
    {
        return 'W';
    }
    else if (strcmp(iconString, "hail") == 0)
    {
        return 'X';
    }
    else if (strcmp(iconString, "thunderstorm") == 0)
    {
        return 'P';
    }
    else {
        return '?';
    }
}