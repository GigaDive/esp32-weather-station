using namespace std;

#include "services/WeekdayConverter.h"

const char *intToGermanWeekday(char dayNumberChar)
{

    int dayNumber = dayNumberChar - '0';
    switch (dayNumber % 7)
    {
    case 0:
        return "Sonntag";
        break;
    case 1:
        return "Montag";
        break;
    case 2:
        return "Dienstag";
        break;
    case 3:
        return "Mittwoch";
        break;
    case 4:
        return "Donnerstag";
        break;
    case 5:
        return "Freitag";
        break;
    case 6:
        return "Samstag";
        break;

    default:
        return "Fehler";
        break;
    }
}