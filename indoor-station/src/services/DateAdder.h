#ifndef DATEADDER
#define DATEADDER

class Date {

public:
    int year;
    int month;
    int day;
};

bool getTomorrow(Date today, Date &tomorrow);

#endif