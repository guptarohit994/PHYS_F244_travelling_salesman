#include "depthfirst.h"
#include <omp.h>
#include "mpi.h"
#define MASTER 0

int numTasks,
	taskID,
	len,
	rc; //return code
char hostname[MPI_MAX_PROCESSOR_NAME];

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
double DFS(int verbosity) {
	struct PathsLL *pathsLL = createPathsLL();
	//saving memory
	//struct PathsLL *completePathsLL = createPathsLL();

	struct Path *bestPath;
	double minCost = HUGE_VALF;
	long numCompletePaths = 0;

	// //create 1st tour
	// struct Path *path = createPath();
	// addCity(path, 0);
	// push(pathsLL, path);

	//max number of threads available per node
	int totalNumThreads = omp_get_max_threads();

	//there are (n-1) starting paths
	//how many cores available?
	int totalCores = numTasks * totalNumThreads;
	//TODO for now, use only n-1 cores


	//total initial paths for distribution
	int numInitialPaths = (n - 1); //numPaths(pathsLL);

	//each task gets these number of paths to divide among its threads
	int initialPathsPerTask = numInitialPaths / numTasks;
	if (taskID == numTasks - 1) {
		//take the remaining residue
		initialPathsPerTask = numInitialPaths - (initialPathsPerTask * (taskID));
	}


	int *shareArray;
	//only transmit the 2nd cities for each path (the ones after 0)
	//create an array for max possible i.e. the last one will have residue
	int shareArrayLen;
	if (taskID == MASTER) {
	 	shareArrayLen = (numInitialPaths - (initialPathsPerTask * (numTasks - 1))) * 1;
	} else {
		shareArrayLen = initialPathsPerTask * 1;
	}
	shareArray = (int*) malloc(shareArrayLen* sizeof(int));
	//shareArray[0] = 0;

	int tasksSent = 0;

	//(n-1) starting paths
	if (taskID == MASTER){
		int *masterArray;
		masterArray = (int*) malloc(numInitialPaths * sizeof(int));

		int i = 0;
		//printf("Process %d has masterArray: ", taskID);
		for (int b=n-1; b>0; b--) {
			*(masterArray+i) = b;
			//printf(" %d(%d)", *(masterArray+i), i);
			i++;
		}
		//printf("\n");

		//printf("Process %d has initialPathsPerTask:%d, shareArrayLen:%d\n", taskID, initialPathsPerTask, shareArrayLen);
		for (int j=1; j<numTasks; j++) {
			for (int k=0; k<initialPathsPerTask; k++) {
				*(shareArray+k) = *(masterArray + j * initialPathsPerTask + k);
			}
			if (j == numTasks - 1) {
				for (int l=0; l<(shareArrayLen - initialPathsPerTask); l++)
					*(shareArray +  initialPathsPerTask + l) = *(masterArray + j * initialPathsPerTask + initialPathsPerTask + l);
				//data, count of data, datatype, destination, tag, communicator
				MPI_Send(shareArray, shareArrayLen, MPI_INT, j, 0, MPI_COMM_WORLD);
				//printf("Process %d sent cities to process %d -", taskID, j);
				//for (int i=0; i < shareArrayLen; i++)
					//printf(" %d", *(shareArray+i));
			} else {
				//data, count of data, datatype, destination, tag, communicator
				MPI_Send(shareArray, initialPathsPerTask, MPI_INT, j, 0, MPI_COMM_WORLD);
				//printf("Process %d sent cities to process %d -", taskID, j);
				//for (int i=0; i < initialPathsPerTask; i++)
					//printf(" %d", *(shareArray+i));
			}
			//printf("\n");
		}
		for (int i=0; i < initialPathsPerTask; i++) {
			//printf(" %d", *(masterArray+i));
			struct Path* tempPath = createPath();
			addCity(tempPath, 0);
			addCity(tempPath, *(masterArray+i));
			push(pathsLL, tempPath);
		}
		//printPathsLL(LOW, pathsLL);
		free(masterArray);

		// //for self
		// shareArray[1] = 1;
	} else{
		//data, count of data, datatype, source, tag, communicator
		MPI_Recv(shareArray, shareArrayLen, MPI_INT, MASTER, 0, MPI_COMM_WORLD,	MPI_STATUS_IGNORE);
		//printf("Process %d received cities from process 0 - ", taskID);
		for (int i=0; i < shareArrayLen; i++) {
			//printf(" %d", *(shareArray+i));
			struct Path* tempPath = createPath();
			addCity(tempPath, 0);
			addCity(tempPath, *(shareArray+i));
			push(pathsLL, tempPath);
		}
		//printf("\n");
		//printPathsLL(LOW, pathsLL);

	}
	free(shareArray);
	
	numInitialPaths = numPaths(pathsLL);

	int numStartingThreads = omp_get_max_threads();
	if (totalNumThreads > numInitialPaths) {
		numStartingThreads = numInitialPaths;
	}

	int pathsPerThread = (int)(ceil((double)numInitialPaths/numStartingThreads));


	printf("taskID:%d, totalNumThreads:%d, numStartingThreads:%d, numInitialPaths:%d, pathsPerThread:%d\n", \
				taskID, totalNumThreads, numStartingThreads, numInitialPaths, pathsPerThread);
	fprintf(outfile_fp, "taskID:%d, totalNumThreads:%d, numStartingThreads:%d, numInitialPaths:%d, pathsPerThread:%d\n", \
							taskID, totalNumThreads, numStartingThreads, numInitialPaths, pathsPerThread);

	double minCost_shared = minCost;
	omp_set_nested(1);
	
	#pragma omp parallel shared(pathsLL, bestPath, pathsPerThread, minCost_shared) \
						 reduction(+:numCompletePaths)\
						 num_threads(numStartingThreads)
	{
		int threadID = omp_get_thread_num();
		struct PathsLL* pvtPathsLL;

		//all these variables only used by master thread. perhaps there is better way to initialize them?
		int buffer = 0;	//buffer variable to store values
		MPI_Request requests[numTasks];
		//initialize requests
		for(int i=0; i<numTasks; i++)
			requests[i]=MPI_REQUEST_NULL;
		const int taskDoneTag=23;	//arbitrary int so long as different from other tags
		const int minCostTag=11;
		double lastSharedMin=minCost_shared;	//most recent minCost_shared value that the tasks shared with each other
		const double updRat=.9;	//if(minCost<updRat*lastSharedMin) it will be shared to the other tasks
		const int recMess=500; //if(count%=recMess == 0) check if any messages to be received
		int sendFlag=0;	//MPI_Iprobe marks as 1 if message ready to be received
		int counter=0;	//how many times while loop has happened
		int tasksDone=0;	//# of tasks finished with their assigned stack
		int finishedTasks[numTasks];	//marks which tasks finished, 1 is finished, 0 is not, index refers to taskID
		//initialize
		for(int i=0; i<numTasks; i++)
			finishedTasks[i]=0;

		#pragma omp critical
		{
			pvtPathsLL = get_my_share(pathsLL, threadID, pathsPerThread);
		}

		
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
					#pragma omp critical
					{
						//update minCost if it is less than the current known value
						if (minCost < minCost_shared) {
							minCost_shared = minCost;
							//#pragma omp flush(minCost_shared)
							//implicit flush at critical ending bracket
						}
					}
					//bestPath = tempPath;
					//print only when new minCost is achieved
					printPath(LOW, tempPath, FALSE);
				}
			} else {
				//#pragma omp parallel for shared(minCost_shared, tempPath, pvtPathsLL) num_threads(innerThreads)
				for (int b=n-1; b>0; b--) {
					// printf("omp_parallel_for: thread %d of the %d children of %d: handling iter %d\n",
			  //                       omp_get_thread_num(), omp_get_team_size(2),
			  //                       omp_get_ancestor_thread_num(1), b);
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

			//need master since used FUNNELED
			#pragma omp master
			{
				counter++;
				counter%=recMess;

				//receive all minCost updates when counter==0
				if(counter==0) {
					sendFlag=0;
					while(!sendFlag) {
						MPI_Iprobe(MPI_ANY_SOURCE, minCostTag, MPI_COMM_WORLD, &sendFlag, MPI_STATUS_IGNORE);	//must change if we ever use a shared partition
						if(sendFlag) {
							MPI_Recv(&minCost, 1, MPI_DOUBLE, MPI_ANY_SOURCE, minCostTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
							if(minCost<minCost_shared) {
								#pragma omp atomic write
									minCost_shared=minCost;
								lastSharedMin=minCost;
							}
							sendFlag=0;
						} else {
							minCost=minCost_shared;
							sendFlag=1;
						}
					}
				}

				if(minCost<updRat*lastSharedMin) {
					//update which tasks are finished
					sendFlag=0;
					while(!sendFlag) {
						MPI_Iprobe(MPI_ANY_SOURCE, taskDoneTag, MPI_COMM_WORLD, &sendFlag, MPI_STATUS_IGNORE);
						if(sendFlag) {
							MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, taskDoneTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
							tasksDone++;
							finishedTasks[buffer]=1;
							sendFlag=0;
						} else {
							sendFlag=1;
						}
					}
					//send new minCost to unfinished tasks
					//what happens if two threads simultaneous send to each other?
					//should I ensure system buffer (not buffer var) large enough to store numTasks doubles?
					//if task finishes after you check for finishedTasks, than Isend will never resolve
					for(int i=0; i<numTasks; i++)
						if(i != taskID && finishedTasks[i] == 0)
							MPI_Isend(&minCost, 1, MPI_DOUBLE, i, minCostTag, MPI_COMM_WORLD, &requests[i]);
					lastSharedMin=minCost;
					MPI_Waitall(numTasks, requests, MPI_STATUS_IGNORE);	//should I wait for all requests or only those that I checked? 
				}
			}//master
		}//while
		freePathLL(pvtPathsLL);
		#pragma omp master
		{
			//tell other tasks you are done so they will stop sending to you
			//currently after master thread finishes it will stop helping the other threads to update minCost_shared from other tasks. consider fixing
			if(tasksDone<numTasks)
				for(int i=0; i<numTasks; i++)
					if(i != taskID)
						MPI_Isend(&taskID, 1, MPI_INT, i, taskDoneTag, MPI_COMM_WORLD, &requests[i]);
			tasksDone++;
			MPI_Waitall(numTasks, requests, MPI_STATUS_IGNORE);	//should I wait for all requests or only those that I checked?
 
			//resolve all messages ready to be received
			while(tasksDone<numTasks) {	
				MPI_Iprobe(MPI_ANY_SOURCE, minCostTag, MPI_COMM_WORLD, &sendFlag, MPI_STATUS_IGNORE);	//must change if we ever use a shared partition
				if(sendFlag)
					MPI_Recv(&minCost, 1, MPI_DOUBLE, MPI_ANY_SOURCE, minCostTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				MPI_Iprobe(MPI_ANY_SOURCE, taskDoneTag, MPI_COMM_WORLD, &sendFlag, MPI_STATUS_IGNORE);
				if(sendFlag) {
					MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, taskDoneTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					tasksDone++;
				}
			}
		}//master
	}//parallel
	printf("taskID:%d, Evaluated a total of %ld feasible complete paths.\n", taskID, numCompletePaths);
	fprintf(outfile_fp, "taskID:%d, Evaluated a total of %ld feasible complete paths.\n", taskID, numCompletePaths);
	//saving memory
	// //print all complete paths
	// printPathsLL(verbosity, completePathsLL);
	freePathLL(pathsLL);

	//}

	//}

	return minCost_shared;
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
	

	int req_lvl=MPI_THREAD_FUNNELED;
	int avail_lvl=-1;
	/* Obtain number of tasks and task ID */
//	MPI_Init(&argc,&argv);
	MPI_Init_thread(&argc, &argv, req_lvl, &avail_lvl);
	MPI_Comm_size(MPI_COMM_WORLD,&numTasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&taskID);
	MPI_Get_processor_name(hostname, &len);
	printf ("MPI task %d has started on %s...\n", taskID, hostname);
	printf("MPI thread level? :%d\n", avail_lvl>=req_lvl);	//one means supported, 0 means not

	char buf[15];
	snprintf(buf, 15, "_%d.txt", taskID);
	if (!strcmp(outfile_name,"")) {
		char default_outfile_name[100] = "./depthfirst_parallel_hybrid";
		strcat(default_outfile_name, buf);
		strncpy(outfile_name, default_outfile_name, 100);
	} else {
		strcat(outfile_name, buf);
	}
	
	outfile_fp = fopen(outfile_name, "w+");
	printf("taskID:%d, Writing the output to log file:\t%s\n",taskID, outfile_name);
	
	//only change the size of buffer when debugging
	#if CVERBOSE > 1
	setBufSize(n);
	#endif

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
	
    printf ("taskID:%d, Number of processors available = %d\n", taskID, omp_get_num_procs());
    fprintf (outfile_fp, "taskID:%d, Number of processors available = %d\n", taskID, omp_get_num_procs());
  	
  	printf ("taskID:%d, Number of threads =              %d\n", taskID, omp_get_max_threads());
  	fprintf (outfile_fp, "taskID:%d, Number of threads =              %d\n", taskID, omp_get_max_threads());

	printAdjacencyMatrix();

	int start = 0;
	
	//startTime = clock();
	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	double wtime = MPI_Wtime();
	//printf("start time:%f\n", wtime);
	
	double minCostLocal = DFS(LOW);
	
	//send_data, receive_data, count of receive data, type of send_data, operation, root process number, communicator
	rc = MPI_Reduce(&minCostLocal, &minCost, 1, MPI_DOUBLE, MPI_MIN, MASTER, MPI_COMM_WORLD);	//is minCost still defined outside of DFS???

	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	wtime = MPI_Wtime () - wtime;
	//printf("total time:%f\n", wtime);
	//endTime = clock();
    //cpu_time_used = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;

    fprintf(outfile_fp, "\n\n\n");
	fprintf(outfile_fp, "=====================================\n");

	if (taskID == MASTER) {

		printf("Lowest Cost:%.2f\n", minCost);
		fprintf(outfile_fp, "Lowest Cost:%.2f\n", minCost);
		//printf("There were %d possible paths.\n", competingPaths);
		printf ( "Wallclock time = %f\n", wtime );
		fprintf (outfile_fp, "Wallclock time = %f\n", wtime );
		
		//following will show the time taken by each used core * num cores used
		//printf("\nTook %.10f seconds to execute\n", cpu_time_used);
		//fprintf(outfile_fp, "\nTook %.10f seconds to execute\n", cpu_time_used);
	}

	free(G);
	fclose(outfile_fp);
	return 0; 
} 
