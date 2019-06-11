#include "depthfirst.h"

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

int greedyNextCityFinder(int visited[], int currentCity) {
	double minCost = HUGE_VALF;
	int nextCity = -1;

	for (int i = 0; i<n; i++) {
		if (i != currentCity && visited[i] != 1) {
			double cost = cityToCityCost(currentCity, i);
			if (cost < minCost) {
				minCost = cost;
				nextCity = i;
			}
		}
	}
	return nextCity;
}


//assumes start point is always zeroth city
double DFS(int verbosity) {
	int visited[n];
	struct Path *path = createPath();
	//init
	for (int i = 0; i<n; i++) {
		visited[i] = 0;
	}

	int startCity = 0;
	int currentCity = startCity;
	double minCost = HUGE_VALF;

	addCity(path, startCity);
	visited[startCity] = 1;

	while (numCities(path) != n) {
		int nextCity = greedyNextCityFinder(visited, currentCity);
		visited[nextCity] = 1;
		
		addCity(path, nextCity);
		currentCity = nextCity;
	}

	addCity(path, startCity);

	printPath(LOW, path, FALSE);
	return pathCost(path);


}

  
/* Driver program to test above functions */
int main(int argc, char *argv[]) 
{ 
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
	printf("Wallclock time = %f\n", cpu_time_used);
	fprintf(outfile_fp, "\nTook %.10f seconds to execute\n", cpu_time_used);

	free(G);
	fclose(outfile_fp);
	return 0; 
} 