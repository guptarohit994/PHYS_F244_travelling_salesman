#include "depthfirst.h"
#include <omp.h>

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
	fprintf(stderr, "Usage: depthfirst_serial <options>\n");
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
		exit(1);
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
		if (index->city == -1) {
			fprintf(stderr, "feasible: City = -1 and next = NULL is present.\n");
			exit(1);
		}
		
		//reached the last/first (only) city
		//check for visited and accumulated cost
		visited[index->city] = 1;
		if (visited[city] == 1) {
			feasible = FALSE;
		} else {
			cost += *(G + (index->city) * n + city);
			if (cost >= minCost && minCost != 0.0) {
				//cnprintfai(FULL, "feasible", "Rejecting the following path due to cost. Going to city:", city);
				//printPath(FULL, path, TRUE);
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
	double minCost = 0.0;
	unsigned long numCompletePaths = 0;

	//create 1st tour
	struct Path *path = createPath();
	addCity(path, 0);
	push(pathsLL, path);
	while (!isEmpty(pathsLL)) {
		struct Path *tempPath = pop(pathsLL);
		int tempPathCityCount = numCities(tempPath);
		if ( tempPathCityCount == n) {
			numCompletePaths += 1;
			//add 0th city for RTT time
			addCity(tempPath, 0);
			double tempPathCost = pathCost(tempPath);
			// print all complete paths
			//printPath(LOW, tempPath, TRUE);
			//saving memory
			// //adding to completePathsLL
			// push(completePathsLL, tempPath);

			if (minCost == 0) {
				//cnprintf(FULL, "DFS", "Found the 1st non-zero minCost");
				minCost = tempPathCost;
				//bestPath = tempPath;
				//print only when new minCost is achieved
				printPath(LOW, tempPath, TRUE);
			} else {
				if (minCost > tempPathCost) {
					minCost = tempPathCost;

					//print only when new minCost is achieved
					printPath(LOW, tempPath, TRUE);
				}
			}
		} else {
			for (int b=n-1; b>0; b--) {
				if (feasible(tempPath, b, minCost) == TRUE) {
					struct Path* newPath = copyPath(tempPath);
					addCity(newPath, b);
					push(pathsLL, newPath);
				}
			}
		}
		freePath(tempPath);
		//printPathsLL(verbosity, pathsLL);
	}//while
	//saving memory
	// //print all complete paths
	// printPathsLL(verbosity, completePathsLL);
	freePathLL(pathsLL);
	printf("Evaluated a total of %ld feasible complete paths.\n", numCompletePaths);
	fprintf(outfile_fp, "Evaluated a total of %ld feasible complete paths.\n", numCompletePaths);
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
		char default_outfile_name[100] = "./depthfirst_serial_out.txt";
		strncpy(outfile_name, default_outfile_name, 100);
	}
	
	outfile_fp = fopen(outfile_name, "w+");
	printf("Writing the output to log file:\t%s\n",outfile_name);

	//only change the size of buffer when debugging
	#if CVERBOSE > 1
	setBufSize(n);
	#endif

	fprintf(outfile_fp, "Dataset file_name: %s\n", file_name);
	fprintf(outfile_fp, "numCities: %d\n", n);

	clock_t startTime, endTime;
    double cpu_time_used;
	
	G = (double*) malloc(n * n * sizeof(double)); 

    char ch;
   	FILE *input_fp;
    input_fp = fopen(file_name, "r"); // read mode
    if (input_fp == NULL) {
		fprintf(stderr, "Error while opening the file.\n");
		fclose(outfile_fp);
		fclose(input_fp);
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
	
	printAdjacencyMatrix();

	int start = 0;
	
	//startTime = clock();
	double wtime = omp_get_wtime ();
	
	minCost = DFS(LOW);

	wtime = omp_get_wtime () - wtime;
	//endTime = clock();
    //cpu_time_used = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;

    fprintf(outfile_fp, "\n\n\n");
	fprintf(outfile_fp, "=====================================\n");

	printf("Lowest Cost:%.2f\n", minCost);
	fprintf(outfile_fp, "Lowest Cost:%.2f\n", minCost);
	//printf("There were %d possible paths.\n", competingPaths);
	printf ( "Wallclock time = %f\n", wtime );
	fprintf (outfile_fp, "Wallclock time = %f\n", wtime );
	//fprintf(outfile_fp, "\nTook %.10f seconds to execute\n", cpu_time_used);

	free(G);
	fclose(outfile_fp);
	return 0; 
} 
