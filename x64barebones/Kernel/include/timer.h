#ifndef TIMER_H
#define TIMER_H

void time_handler();
unsigned long long ticks_elapsed();
unsigned long long seconds_elapsed();
unsigned long long ms_elapsed();
void sleep(unsigned long long ms);
void sleepSeconds(unsigned long long seconds);

#endif
