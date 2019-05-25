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
	double minCost = 0.0;

	//create 1st tour
	struct Path *path = createPath();
	addCity(path, 0);
	push(pathsLL, path);
	#pragma omp parallel shared(pathsLL, minCost, bestPath)
	{
	#pragma omp single
	{

	while (!isEmpty(pathsLL)) {
		#pragma omp task untied
		{

		struct Path *tempPath;
		#pragma omp critical
		{
			tempPath = pop(pathsLL);
		}
		int tempPathCityCount = numCities(tempPath);
		if ( tempPathCityCount == n) {
			//add 0th city for RTT time
			addCity(tempPath, 0);
			double tempPathCost = pathCost(tempPath);
			//saving memory
			// //adding to completePathsLL
			// push(completePathsLL, tempPath);
			#pragma omp critical	//perhaps use something besides critical? can push and pull while doing this
			{

			if (minCost == 0 || minCost>tempPathCost) {
				minCost = tempPathCost;
				//bestPath = tempPath;
				//print only when new minCost is achieved
				printPath(LOW, tempPath, FALSE);
			}

			}
		} else if (numCities != 0) {
			#pragma omp for schedule(dynamic)
			{

			for (int b=n-1; b>=0; b--) {
				if (feasible(tempPath, b, minCost) == TRUE) {
					struct Path* newPath = copyPath(tempPath);
					addCity(newPath, b);
					#pragma omp critical
					{
						push(pathsLL, newPath);
					}
				}
			}
			
			}
		}
		freePath(tempPath);
		//printPathsLL(verbosity, pathsLL);
		
		}

	}//while
	//saving memory
	// //print all complete paths
	// printPathsLL(verbosity, completePathsLL);
	freePathLL(pathsLL);

	}

	}
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
		printf("Using default outfile_name:\"%s\"\n",outfile_name);
	}
	
	outfile_fp = fopen(outfile_name, "w+");
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
	
	startTime = clock();
	
	minCost = DFS(LOW);
	//stackSelfTest();

	endTime = clock();
    cpu_time_used = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;

    fprintf(outfile_fp, "\n\n\n");
	fprintf(outfile_fp, "=====================================\n");

	printf("Lowest Cost:%.2f\n", minCost);
	fprintf(outfile_fp, "Lowest Cost:%.2f\n", minCost);
	//printf("There were %d possible paths.\n", competingPaths);
	printf("\nTook %.10f seconds to execute\n", cpu_time_used);
	fprintf(outfile_fp, "\nTook %.10f seconds to execute\n", cpu_time_used);

	free(G);
	fclose(outfile_fp);
	return 0; 
} 