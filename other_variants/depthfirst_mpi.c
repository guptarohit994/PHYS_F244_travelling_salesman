#include "depthfirst.h"
#include "mpi.h"
#define MASTER 0

int numTasks,
	taskID,
	rc; //return code

int handle_option(char *arg) {
	if (!strncmp(arg,"--dataset=",10)) {
		sscanf(arg+10,"%s", file_name); 
	} else if (!strncmp(arg,"--cities=",9)) {
		sscanf(arg+9,"%d", &n);
	} else if (!strncmp(arg,"--outfile=",10)) {
		sscanf(arg+10,"%s", outfile_name);
	} else {
		return 0;
	}
	return 1;
}

void usage() {
	fprintf(stderr, "Usage: depthfirst_parallel <options>\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "--help				Print this message\n");
	fprintf(stderr, "--dataset=<file>	path to the file containing the intercity distance table.\n");
	fprintf(stderr, "--cities=n 		number of cities being considered in the dataset\n");
	fprintf(stderr, "--outfile=n 		path of the output file\n");
}

//returns whether path is feasible. ie: whether went to same city twice (unless is starting city and path is complete),
//whether path is empty, whether cost of path less than minCost of complete paths (perhaps do this in DFS instead), etc.
int feasible(struct Path *path, int city, double minCost) {
	int feasible = TRUE;

	int visited[n];
	//init
	for (int i = 0; i<n; i++) {
		visited[i] = 0;
	}
	if(pathEmpty(path)) {
		fprintf(stderr, "feasible: Empty path should not be passed here.\n");
		feasible = FALSE;
		return feasible;
	} else {
		struct Path *index = path;

		double cost = 0.0;
		while(index->next != NULL) {
			visited[index->city] = 1;
			cost += *(G + (index->city) * n + index->next->city);
			//this condition seems unreachable. TODO check it
			if (cost >= minCost && minCost != 0.0) {
				feasible = FALSE;
				return feasible;
			}
			index=index->next;
		}
			
		//when at last node/check for paths with just a single city, check for visited and accumulated cost
		visited[index->city] = 1;
		if (visited[city] == 1) {
			feasible = FALSE;
		} else {
			cost += *(G + (index->city) * n + city);
			if (cost >= minCost && minCost != 0.0) {
				feasible = FALSE;
			}
		}
	}
	return feasible;
}

//assumes start point is always zeroth city
double DFS(int verbosity, int taskID) {
	struct PathsLL *pathsLL = createPathsLL();
	//saving memory
	//struct PathsLL *completePathsLL = createPathsLL();

	struct Path *bestPath;
	double minCost = HUGE_VALF;
	long numCompletePaths = 0;

	//create 1st tour
	struct Path *path = createPath();
	addCity(path, 0);
	push(pathsLL, path);

	double minCost_shared = minCost;
	int *shareArray;
	shareArray = (int*) malloc(2*sizeof(int));
	*(shareArray) = 0;

	if (taskID == MASTER){
		for (int b=n-1; b>1; b--) {
			*(shareArray+1) = b;
			//data, count of data, datatype, destination, tag, communicator
			MPI_Send(shareArray, 2, MPI_INT, b-1, 0, MPI_COMM_WORLD);
			printf("Process %d sent cities %d, %d to process %d\n", taskID, *(shareArray), *(shareArray+1), b-1);
		}
		//for self
		shareArray[1] = 1;
	} else if (taskID != MASTER && taskID < (n - 1)) {
		//data, count of data, datatype, source, tag, communicator
		MPI_Recv(shareArray, 2, MPI_INT, MASTER, 0, MPI_COMM_WORLD,	MPI_STATUS_IGNORE);
		printf("Process %d received cities %d, %d from process 0\n", taskID, *(shareArray), *(shareArray+1));
	} else {
		//do nothing
	}

	/*
	if (taskID < (n-1)) {
		struct PathsLL* pvtPathsLL = pathsLL;
		addCity(path, shareArray[1]);

		while (!isEmpty(pvtPathsLL)) {
			struct Path* tempPath;
			//printf("Thread:%d is running this task\n", omp_get_thread_num());
		
			//printf("Thread:%d is running this task\n", omp_get_thread_num());
			tempPath = pop(pvtPathsLL);
			//printPath(LOW, tempPath, FALSE);

			int tempPathCityCount = numCities(tempPath);
			if ( tempPathCityCount == n) {
				//add 0th city for RTT time
				numCompletePaths += 1;
				//printf("thread:%d, Found a complete path\n", omp_get_thread_num());
				addCity(tempPath, 0);
				double tempPathCost = pathCost(tempPath);
				//saving memory
				// //adding to completePathsLL
				// push(completePathsLL, tempPath);
				if (minCost>tempPathCost) {
					minCost = tempPathCost;
					
					//bestPath = tempPath;
					//print only when new minCost is achieved
					printPath(LOW, tempPath, FALSE);
				}
			} else {
				for (int b=n-1; b>0; b--) {
					if (feasible(tempPath, b, minCost_shared) == TRUE) {
						struct Path* newPath = copyPath(tempPath);
						addCity(newPath, b);
						push(pvtPathsLL, newPath);
					} else {
						//printf("thread:%d, rejecting a mini-path\n", omp_get_thread_num());
					}
				}
			
			}	
			freePath(tempPath);
			//printPathsLL(verbosity, pathsLL);
		}//while
		
		freePathLL(pvtPathsLL);
	}
	*/
	printf("taskID:%d, Evaluated a total of %ld feasible complete paths.\n", taskID, numCompletePaths);
	fprintf(outfile_fp, "taskID:%d, Evaluated a total of %ld feasible complete paths.\n", taskID, numCompletePaths);
	//saving memory
	// //print all complete paths
	// printPathsLL(verbosity, completePathsLL);
	//freePathLL(pathsLL);

	//}

	//}
	return minCost;
}

//main starts here
int main(int argc, char *argv[]) { 

	//process command line args
	for (int i = 1; i<argc; i++) {
		if(!strcmp(argv[i],"--help")) {
			usage();
			exit(0);
		} else if (!strncmp(argv[i],"--",2)) {
			if (!handle_option(argv[i])) {
				printf("Error! Unrecognized option %s\n", argv[i]);
				usage();
				exit(1);
			}
		} else {
			//unhandled case
			printf("Error! unhandled case\n");
			exit(1);
		}
	}
	if (!strcmp(file_name,"")) {
		printf("Error! dataset option invalid with value:\"%s\"\n",file_name);
		usage();
		exit(1);
	}
	if (n <= 0) {
		printf("Error! cities option invalid with value:\"%d\"\n",n);
		usage();
		exit(1);
	}
	
	/* Obtain number of tasks and task ID */
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskID);
	printf ("MPI task %d has started...\n", taskID);

	char default_outfile_name[100] = "./depthfirst_parallel_out";
	char buf[15];
	snprintf(buf, 15, "_%d.txt", taskID);
	strcat(default_outfile_name, buf);
	strncpy(outfile_name, default_outfile_name, 100);


	outfile_fp = fopen(outfile_name, "w+");
	printf("taskID:%d, Writing the output to log file:\t%s\n",taskID, outfile_name);
	
	//only change the size of buffer when debugging
	//#if CVERBOSE > 1
	setBufSize(n);
	//#endif

	fprintf(outfile_fp, "taskID:%d, Dataset file_name: %s\n", taskID, file_name);
	fprintf(outfile_fp, "taskID:%d, numCities: %d\n", taskID, n);

	clock_t startTime, endTime;
    double cpu_time_used;
	
	G = (double*) malloc(n * n * sizeof(double)); 

	// //replicate across the diagonal
	// for (int i = 0; i<n; i++) {
	// 	for (int j = i; j<n; j++) {
	// 		G[j][i] = G[i][j];
	// 	}
	// }
    char ch;
   	FILE *input_fp;
    input_fp = fopen(file_name, "r"); // read mode
    if (input_fp == NULL) {
		fprintf(stderr, "Error while opening the file.\n");
		fclose(outfile_fp);
		exit(1);
   	}

   	double numberArray[n*n];
   	for (int i = 0; i < n*n; i++){
        fscanf(input_fp, "%lf", &numberArray[i] );
    }
   	fclose(input_fp);
   	for (int i = 0; i < n*n; i++){
   		*(G + i) = numberArray[i];
        // printf("Number:%lf\n", numberArray[i]);
    }

    if (taskID == MASTER) {
    	printf ("Number of processors available = %d\n", numTasks);
    }

    
    fprintf (outfile_fp, "Number of processors available = %d\n", numTasks);
  	
  	//printf ("Task Number =              %d\n", taskID);
  	fprintf (outfile_fp, "Task Number =              %d\n", taskID);

  	

	printAdjacencyMatrix();

	int start = 0;
	
	startTime = clock();
	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	double wtime = MPI_Wtime();
	
	double minCostLocal = DFS(LOW, taskID);

	//send_data, receive_data, count of receive data, type of send_data, operation, root process number, communicator
	rc = MPI_Reduce(&minCostLocal, &minCost, 1, MPI_DOUBLE, MPI_MIN, MASTER, MPI_COMM_WORLD);
	
	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	wtime = MPI_Wtime () - wtime;
	endTime = clock();
    cpu_time_used = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;

    fprintf(outfile_fp, "\n\n\n");
	fprintf(outfile_fp, "=====================================\n");

    if (taskID == MASTER) {

		printf("Lowest Cost:%.2f\n", minCost);
		fprintf(outfile_fp, "Lowest Cost:%.2f\n", minCost);
		//printf("There were %d possible paths.\n", competingPaths);
		printf ( "Wallclock time = %f\n", wtime );
		fprintf (outfile_fp, "Wallclock time = %f\n", wtime );
		
		//following will show the time taken by each used core * num cores used
		printf("\nTook %.10f seconds to execute\n", cpu_time_used);
		fprintf(outfile_fp, "\nTook %.10f seconds to execute\n", cpu_time_used);
	}

	free(G);
	fclose(outfile_fp);
	MPI_Finalize();
	return 0; 
} 