#include "depthfirst.h"
#include <omp.h>
#include <unistd.h>
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
		//fprintf(stderr, "feasible: Empty path should not be passed here.\n");
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

	//create 1st tour
	struct Path *path = createPath();
	addCity(path, 0);
	push(pathsLL, path);

	//first iteration
	struct Path *tempPath;
	tempPath = pop(pathsLL);
	for (int b=n-1; b>0; b--) {
		if (feasible(tempPath, b, minCost) == TRUE) {
			struct Path* newPath = copyPath(tempPath);
			addCity(newPath, b);
			push(pathsLL, newPath);
		}
	}
	freePath(tempPath);
	
	//max number of threads available
	int totalNumThreads = omp_get_max_threads();
	//start parallel region with these # of threads
	int numStartingThreads = totalNumThreads;
	//total initial paths for distribution
	int numInitialPaths = numPaths(pathsLL);
/*
	if (totalNumThreads > numInitialPaths) {
		numStartingThreads = numInitialPaths;
	}
  */
	int pathsPerThread = (int)(ceil((double)numInitialPaths/numStartingThreads));
	printf("totalNumThreads:%d, numStartingThreads:%d, numInitialPaths:%d, pathsPerThread:%d\n", \
				totalNumThreads, numStartingThreads, numInitialPaths, pathsPerThread);
	fprintf(outfile_fp, "totalNumThreads:%d, numStartingThreads:%d, numInitialPaths:%d, pathsPerThread:%d\n", \
							totalNumThreads, numStartingThreads, numInitialPaths, pathsPerThread);
	double minCost_shared = minCost;

  struct PathsLL* pvtPathsLL=pathsLL;
  int counter=0;
	#pragma omp parallel shared(minCost, pvtPathsLL) \
						 private(tempPath) \
						 num_threads(numStartingThreads)
	{
    #pragma omp single
    {

   //   while(!isEmpty(pvtPathsLL) && counter<totalNumThreads+20) {
      while(!isEmpty(pvtPathsLL)) {
       // if(counter<totalNumThreads+200) {
        #pragma omp task shared(minCost, pvtPathsLL) private(tempPath)
        {
          //printf("Thread:%d is running this task\n", omp_get_thread_num());
    //      #pragma omp critical(updateStack) 
      //    {
            //printf("Thread:%d is running this task\n", omp_get_thread_num());
            tempPath = pop(pvtPathsLL);
            //printPath(LOW, tempPath, FALSE);
       //   }

          int tempPathCityCount = numCities(tempPath);
          if ( tempPathCityCount == n) {
            //add 0th city for RTT time
            addCity(tempPath, 0);
            double tempPathCost = pathCost(tempPath);
            //saving memory
            // //adding to completePathsLL
            // push(completePathsLL, tempPath);
            #pragma omp critical(updateMin)
            {
            if (minCost>tempPathCost) {
              minCost = tempPathCost;
              //update minCost if it is less than the current known value
   //             #pragma omp flush(minCost_shared)	//flush vs critical?
              printPath(LOW, tempPath, FALSE);
            }
            }//critical
          } else if (!pathEmpty(tempPath)) {
            //#pragma omp parallel for schedule(dynamic) shared(minCost, tempPath, pvtPathsLL)
            for (int b=n-1; b>0; b--) {
              if (feasible(tempPath, b, minCost) == TRUE) {
                struct Path* newPath = copyPath(tempPath);
                addCity(newPath, b);
      //          #pragma omp critical(updateStack)
        //        {
                push(pvtPathsLL, newPath);
          //      }
              }
            }
          
          }	
          freePath(tempPath);
          //printPathsLL(verbosity, pathsLL);      
        /*  #pragma omp critical
          { 
          counter--;
          }
         */ 
        }//task
   /*     #pragma omp critical
        {
        counter++;
        }
        }//if
        else {
          sleep(1);
          cnprintf(FULL, "DFS", "delay");
        }//else
     */
      }//while?
    }//single?
	}//parallel
	//saving memory
	// //print all complete paths
	// printPathsLL(verbosity, completePathsLL);
	//freePathLL(pathsLL);
	freePathLL(pvtPathsLL);

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
	if (!strcmp(outfile_name,"")) {
		char default_outfile_name[100] = "./depthfirst_parallel_out.txt";
		strncpy(outfile_name, default_outfile_name, 100);
	}
	
	outfile_fp = fopen(outfile_name, "w+");
	printf("Writing the output to log file:\t%s\n",outfile_name);
	
	//only change the size of buffer when debugging
//	#if CVERBOSE > 1
	setBufSize(n);
//	#endif

	fprintf(outfile_fp, "Dataset file_name: %s\n", file_name);
	fprintf(outfile_fp, "numCities: %d\n", n);

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
	
    printf ("Number of processors available = %d\n", omp_get_num_procs());
    fprintf (outfile_fp, "Number of processors available = %d\n", omp_get_num_procs());
  	
  	printf ("Number of threads =              %d\n", omp_get_max_threads());
  	fprintf (outfile_fp, "Number of threads =              %d\n", omp_get_max_threads());

	printAdjacencyMatrix();

	int start = 0;
	
	startTime = clock();
	double wtime = omp_get_wtime ();
	
	minCost = DFS(LOW);
//	stackSelfTest();
	wtime = omp_get_wtime () - wtime;
	endTime = clock();
    cpu_time_used = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;

    fprintf(outfile_fp, "\n\n\n");
	fprintf(outfile_fp, "=====================================\n");

	printf("Lowest Cost:%.2f\n", minCost);
	fprintf(outfile_fp, "Lowest Cost:%.2f\n", minCost);
	//printf("There were %d possible paths.\n", competingPaths);
	printf ( "Wallclock time = %f\n", wtime );
	fprintf (outfile_fp, "Wallclock time = %f\n", wtime );
	
	//following will show the time taken by each used core * num cores used
	printf("\nTook %.10f seconds to execute\n", cpu_time_used);
	fprintf(outfile_fp, "\nTook %.10f seconds to execute\n", cpu_time_used);

	free(G);
	fclose(outfile_fp);
	return 0; 
} 
