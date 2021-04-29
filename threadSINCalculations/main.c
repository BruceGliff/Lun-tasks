#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "task.h"
#include "cpu_optimization.h"


int main(int argc, const char* argv[]) {
	int const thNumber = getThreadNumber(argc, argv);

	Interval * intervals = createIntervalSegmentation();

	pthread_t * threads = malloc(sizeof(pthread_t) * thNumber);
	double * results = malloc(sizeof(double) * thNumber);
	
	for (int i = 0; i != thNumber; ++i) {
		Task * task = malloc(sizeof(Task));
		task->id = i;
		task->thNumber = thNumber;
		task->ranges = intervals;
		task->result = results + i;

		pthread_create(threads + i, NULL, cRoutine, task);
	}

	double res = 0;
	for (int i = 0; i != thNumber; ++i) {
		pthread_join(threads[i], NULL);
		res += results[i];
	}

	destroyIntervalSegmentation(intervals);	
	free(threads);
	free(results);

	printf("Result: %lf\n", res);
	return 0;
}