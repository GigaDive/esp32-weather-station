using namespace std;

#include "services/WeekdayConverter.h"

const char *intToGermanWeekday(int dayNumber) {
    switch (dayNumber % 7) {
        case 0:
            return "Sonntag";
        case 1:
            return "Montag";
        case 2:
            return "Dienstag";
        case 3:
            return "Mittwoch";
        case 4:
            return "Donnerstag";
        case 5:
            return "Freitag";
        case 6:
            return "Samstag";
        default:
            return "Fehler";
    }
}
