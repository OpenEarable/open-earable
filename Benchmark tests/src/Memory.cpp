#include "Memory.h"

// Stack just approximated

unsigned int heap_min = 875;
unsigned int heap_max = 217069 + heap_min; // 216664 - 8
mbed_stats_heap_t heap_stats;

void refresh_stats() {
    mbed_stats_heap_get(&heap_stats);
}

void print_memory() {
    refresh_stats();
    Serial.println("Heap: " + String(heap_stats.current_size-heap_min) + " / " + String(heap_stats.reserved_size-heap_min) + " bytes");
}

void print_percent() {
    refresh_stats();
    Serial.println("Full: " + String(get_percent() * 100) + " %");
}

void print_preallocated() {
    Serial.println("Preallocated: " + String(get_preallocated()) + " bytes");
}


unsigned int get_current(bool refresh) {
    if (refresh) refresh_stats();
    return heap_stats.current_size-heap_min;
}

unsigned int get_max(bool refresh) {
    if (refresh) refresh_stats();
    return heap_stats.max_size-heap_min;
}

float get_percent(bool refresh) {
    if (refresh) refresh_stats();
    return (float)(heap_stats.current_size-heap_min)/(float)(heap_stats.reserved_size-heap_min);
}

unsigned int get_preallocated(bool refresh) {
    if (refresh) refresh_stats();
    return heap_max - heap_stats.reserved_size;
}
