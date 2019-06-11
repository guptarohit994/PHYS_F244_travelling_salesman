#include <omp.h>
#include "mpi.h"
#include <math.h>
#include <stdio.h>


#define MASTER 0

int numTasks,
	taskID,
	len,
	rc; //return code
char hostname[MPI_MAX_PROCESSOR_NAME];

int dfs(int taskid) {
	int *tour;
	tour = (int*) malloc(8 * sizeof(int));

	int data = 0;
	if (taskid == MASTER)
	 	data = 9;
    MPI_Request req;
	MPI_Status status;
	MPI_Bcast(&data, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
	//MPI_Wait(&req, &status);
	printf("taskID:%d data:%d\n", taskid, data);

	if (taskid == MASTER) {
		*(tour+0) = 0;
		*(tour+1) = 1;
		*(tour+2) = 2;
		*(tour+3) = 3;
		*(tour+4) = 4;
		*(tour+5) = 5;
		*(tour+6) = 6;
		*(tour+7) = 7;
	}

	int perNodeCount = (int)(ceil((double)8/3));
	int *displ, *sendcounts;
	displ = (int*) malloc(3 * sizeof(int));
	sendcounts = (int*) malloc(3 * sizeof(int));

	int total = 8;
	int rem = total;

	if (taskid == MASTER) {
		for (int i=0; i<3; i++) {
			*(displ+i) = 3*i;
			if (i != 2) {
				*(sendcounts+i) = perNodeCount;
				rem -= perNodeCount;
			}else {
				*(sendcounts+i) = rem;
			}
			printf("i:%d, displ:%d, sendcounts:%d\n",i, *(displ+i), *(sendcounts+i));
		}


		// printf("Process %d sent cities %d, %d\n", taskid, tour[0], tour[1]);
		// //data, count of data, datatype, destination, tag, communicator
	 //    MPI_Send(&tour, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);

	 //    int data = taskid;
	 //    MPI_Request req;
		// MPI_Status status;
		// MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, &req);
		// MPI_Wait(&req, &status);

	    // return 2;
	} 
	// else if (taskid == 1) {
	//     MPI_Recv(&tour, 2, MPI_INT, 0, 0, MPI_COMM_WORLD,	MPI_STATUS_IGNORE);
	//     printf("Process 1 received cities %d, %d from process 0\n", *(tour), *(tour+1));
	    
	//  //    MPI_Request req = MPI_REQUEST_NULL;
 //  //       MPI_Status status;
 //  //       MPI_Ibcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD, &req);
	// 	// int flag = 0;
	// 	// do {
	// 	// 	//check if any message is pending
	// 	// 	MPI_Test(&request,&flag,&status);
	// 	// } while (flag != 1)	
	// 	// printf("taskID:%d received %d\n", );

	//     return (tour[0] + tour[1	]);
	// } else {
	// 	return taskid;
	// }

	int *revBuf = (int*) malloc(perNodeCount * sizeof(int));
	int recCount = perNodeCount;

	MPI_Scatterv(tour, sendcounts, displ, MPI_INT, revBuf, recCount, MPI_INT, MASTER, MPI_COMM_WORLD);

	printf("taskid:%d got recCount:%d\n", taskid, recCount);
	for (int i=0; i<recCount; i++){
		printf("taskid:%d got %d at i:%d\n", taskid, *(revBuf+i), i);
	}
	return taskid;
}

int main(int argc, char *argv[]) {
	int sum; 
	double start, end;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskID);
	MPI_Get_processor_name(hostname, &len);
	printf ("Hello from task %d on %s!\n", taskID, hostname);


	// printf ("taskID:%d, Number of processors available = %d\n", taskID, omp_get_num_procs());
 //  	printf ("taskID:%d, Number of threads =              %d\n", taskID, omp_get_max_threads());
	
	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	start = MPI_Wtime();

	int x = dfs(taskID);

	//send_data, receive_data, count of receive data, type of send_data, operation, root process number, communicator
	rc = MPI_Reduce(&x, &sum, 1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	end = MPI_Wtime();

	if (taskID == MASTER) {
	   printf("MASTER: the reduced sum is %d\n",sum);
	   printf("Runtime = %f\n", end-start);
	}
	MPI_Finalize();


}