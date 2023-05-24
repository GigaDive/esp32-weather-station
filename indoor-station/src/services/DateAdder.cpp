using namespace std;

#include "DateAdder.h"

static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool valid_date(Date today) {
    // Check month first.
    if (today.month < 1 || today.month > 12)
        return false;

    // Allow Feb 29 in leap year if needed.
    if (today.month == 2 && today.day == 29) {
        if (today.year % 400 == 0)
            return true;
        if ((today.year % 4 == 0) && (today.year % 100 != 0))
            return true;
    }

    // Then check day.
    if (today.day < 1 || today.day > days[today.month - 1])
        return false;

    return true;
}

bool getTomorrow(Date today, Date &tomorrow) {
    // Don't do anything for bad dates.
    if (!valid_date(today))
        return false;

    // Just blindly add a day with no checks.
    tomorrow.year = today.year;
    tomorrow.month = today.month;
    tomorrow.day = today.day + 1;

    // Allow Feb 29 in leap year if needed.
    if (tomorrow.month == 2 && tomorrow.day == 29) {
        if (tomorrow.year % 400 == 0)
            return true;
        if ((tomorrow.year % 4 == 0) && (tomorrow.year % 100 != 0))
            return true;
    }

    // Catch rolling into new month.
    if (tomorrow.day > days[tomorrow.month - 1]) {
        tomorrow.day = 1;
        tomorrow.month++;

        // Catch rolling into new year.
        if (tomorrow.month == 13) {
            tomorrow.month = 1;
            tomorrow.year++;
        }
    }

    return true;
}