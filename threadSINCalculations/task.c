#include <stdlib.h>
#include <math.h>

#include "task.h"

double const LEFT_CORNER = 0.001;
double const RIGHT_CORNER = 1.0;

int const SEGMENT_NUMBER = 20;
int const SEGMENTATION = 100000000;


double function(double value) {
	return sin(1/value);
}

Interval * createIntervalSegmentation() {
	int i = 0;
	double length = RIGHT_CORNER - LEFT_CORNER;

	Interval* range = malloc(sizeof(Interval) * SEGMENT_NUMBER);

	for (i = SEGMENT_NUMBER - 1; i >= 0; --i) {
		range[i].right = LEFT_CORNER + length;
		if (i == 0) {
			range[i].left = LEFT_CORNER;
		} else {
			length /= 2;
			range[i].left = LEFT_CORNER + length;
		}
	}

    return range;
}
void destroyIntervalSegmentation(Interval * invl) {
    free(invl);
}

double cOneInterval(double left, double right) {
	double res = 0;
	double const step = (right - left) / SEGMENTATION;

	for (int i = 1; i != SEGMENTATION; ++i)
		res += step * (function(left + i * step) + function(left + (i - 1) * step)) / 2;	
    return res;
}


void* cRoutine(void* arg) {
	Task* task = (Task*) arg;

	double res = 0;

	int index = task->id;
	int thNumber = task->thNumber;
	for (; index < SEGMENT_NUMBER; index += thNumber) {
		res += cOneInterval(task->ranges[index].left, task->ranges[index].right);
	}
	
	*(task->result) = res;

	free(task);
	return NULL;
}