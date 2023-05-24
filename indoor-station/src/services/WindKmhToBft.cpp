using namespace std;

#include "services/WindKmhToBft.h"

int kmhToBft(float kmh) {
    if (kmh < 1) {
        return 0;
    } else if (kmh < 5) {
        return 1;
    } else if (kmh < 11) {
        return 2;
    } else if (kmh < 19) {
        return 3;
    } else if (kmh < 28) {
        return 4;
    } else if (kmh < 38) {
        return 5;
    } else if (kmh < 49) {
        return 6;
    } else if (kmh < 61) {
        return 7;
    } else if (kmh < 74) {
        return 8;
    } else if (kmh < 88) {
        return 9;
    } else if (kmh < 102) {
        return 10;
    } else if (kmh < 117) {
        return 11;
    } else {
        return 12;
    }
}