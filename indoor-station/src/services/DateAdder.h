#ifndef DATEADDER
#define DATEADDER

// Declaration of Date class
class Date {

    public:
    int year;
    int month;
    int day;
};


bool getTomorrow(Date today, Date &tomorrow);

#endif