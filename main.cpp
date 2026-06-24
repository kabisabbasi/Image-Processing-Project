
#include "app.h"

using namespace std;

int main()
{

   App app;
   app.run();

   return 0;
}






























// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h> // For sleep()

// // Function that simulates a student doing tasks
// void* student_task(void* arg) {
// 	int student_id = *((int*)arg);

// 	printf("Student %d is starting their tasks.\n", student_id);

// 	// Simulate doing multiple tasks
// 	for (int i = 1; i <= 3; i++) {
// 		printf("Student %d is doing task %d.\n", student_id, i);
// 		sleep(1); // Simulate time taken to do a task
// 	}

// 	printf("Student %d has completed all tasks.\n", student_id);

// 	pthread_exit(NULL); // Exit the thread
// }

// int main() {
// 	const int num_students = 5; // Number of student threads
// 	pthread_t threads[num_students];
// 	int student_ids[num_students];

// 	// Create threads for each student
// 	for (int i = 0; i < num_students; i++) {
// 		student_ids[i] = i + 1;
// 		int rc = pthread_create(&threads[i], NULL, student_task, (void*)&student_ids[i]);
// 		if (rc) {
// 			fprintf(stderr, "Error: pthread_create() failed for student %d\n", i + 1);
// 			exit(EXIT_FAILURE);
// 		}
// 	}

// 	// Wait for all threads to complete
// 	for (int i = 0; i < num_students; i++) {
// 		pthread_join(threads[i], NULL);
// 	}

// 	printf("All students have completed their tasks.\n");

// 	return 0;
// }
