#ifndef TASK_H
#define TASK_H

typedef struct _Interval {
	double left;
	double right;
} Interval;
typedef struct _RoutineTask {
	int id;
	int thNumber;
	Interval* ranges;

	double* result;
} Task;

Interval * createIntervalSegmentation();
void destroyIntervalSegmentation(Interval * invl);

void* cRoutine(void* arg);

#endif //TASK_H