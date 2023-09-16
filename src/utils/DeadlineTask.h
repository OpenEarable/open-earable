#ifndef DEADLINE_TASK_H
#define DEADLINE_TASK_H

class DeadlineTask {
public:
    DeadlineTask(unsigned long deadline, void (*task)());
    ~DeadlineTask();
    
    void call();
    unsigned long deadline;
private:
    void (*_task)();
};

#endif