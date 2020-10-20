#ifndef TIMEMODULE_H
#define TIMEMODULE_H

// ----------------------------------------------------------------------------

#include <Time.h>
// ----------------------------------------------------------------------------


tmElements_t tm;

const char *monthName[12] = {
    "Jan",
    "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void TimeModule_init();
void TimeModule_getDate();
void TimeModule_getTime

#endif