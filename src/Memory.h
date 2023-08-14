#ifndef TECO_ML_MEMORY_H
#define TECO_ML_MEMORY_H

#include "Arduino.h"

#include "mbed.h"
#include "mbed_mem_trace.h"

// add macros to mbed.json
/*
  "MBED_HEAP_STATS_ENABLED=1",
  "MBED_STACK_STATS_ENABLED=1"
 */

extern unsigned int heap_min;
extern unsigned int heap_max;
extern mbed_stats_heap_t heap_stats;

void refresh_stats();

void print_memory();
void print_percent();
void print_preallocated();

unsigned int get_current(bool refresh=true);
unsigned int get_max(bool refresh=true);
float get_percent(bool refresh=true);

unsigned int get_preallocated(bool refresh=true); // Approximated


#endif //TECO_ML_MEMORY_H

