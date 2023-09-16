#include "DeadlineTask.h"

static void empty_task() {}

DeadlineTask::DeadlineTask(unsigned long deadlin, void (*task)()) {
    this->deadline = deadline;
    this->_task = task;
}

void DeadlineTask::call() {
    _task();
    _task = empty_task;
}

