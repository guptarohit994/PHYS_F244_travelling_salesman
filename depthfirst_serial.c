// C program for array implementation of stack 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <time.h>
#include <string.h>

//verbosity levels
#define LOW 1
#define MEDIUM 2
#define FULL 3
#define CVERBOSE LOW
#define cnprintf(lvl,caller,str) ((CVERBOSE>=lvl) ? printf("%s: %s\n",caller,str) : 0);
#define cnprintfa(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? printf("%s: %s = %.2lf\n",caller,str,arg) : 0);
//custom print with one argument (integer)
#define cnprintfai(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? printf("%s: %s = %d\n",caller,str,arg) : 0);
//custom print with two arguments (double)
#define cnprintfaa(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? printf("%s: %s = %.2lf:%.2lf\n",caller,str,arg1,arg2) : 0);
//custom print with first argument (integer) and second argument double
#define cnprintfaia(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? printf("%s: %s = %d:%.2lf\n",caller,str,arg1,arg2) : 0);

#define cprintf(lvl,caller,str) ((CVERBOSE>=lvl) ? printf("%s",str) : 0);

#define cprintfa(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? printf("%s %.2lf",str,arg) : 0);

#define cprintfaia(lvl,caller,str,arg1, arg2) ((CVERBOSE>=lvl) ? printf("%s %d(%.2lf)",str,arg1, arg2) : 0);
//custom print with an argument (integer)
#define cprintfai(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? printf("%s %d",str,arg) : 0);
//custom print with two arguments (integer)
#define cnprintfaai(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? printf("%s: %s = %d:%d\n",caller,str,arg1,arg2) : 0);

#define TRUE 1
#define FALSE 0

double* G;
int n;    //n is no of cities
double minCost = 0.0;
int competingPaths = 0;
char file_name[100];


int handle_option(char *arg) {
	if (!strncmp(arg,"--dataset=",10)) {
		sscanf(arg+10,"%s", file_name); 
	} else if (!strncmp(arg,"--cities=",9)) {
		sscanf(arg+9,"%d", &n);
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
}

double cityToCityCost(int fromCity, int toCity) {
	return *(G + (fromCity) * n + toCity);
}

//Linked List of path taken
struct Path
{
	//TODO: add pathID
	int city;
	double costTillNow;
	struct Path *next;
};

//initializes city to -1. empty path is when city is -1
struct Path* createPath()
{
	cnprintf(FULL, "createPath", "called");
	struct Path* path = (struct Path*) malloc(sizeof(struct Path));
	path->city = -1;
	path->next = NULL;
	path->costTillNow = 0.0;
	return path;
}

//prints path if CVERBOSE >= verbosity
void printPath(int verbosity, struct Path* path, int silent)
{
	if (silent == FALSE) {
		cnprintf(verbosity,"printPath", "Printing path =>");
	}
	struct Path *index = path;

	while(index != NULL) {
		cprintfaia(verbosity,"printPath", "", index->city, index->costTillNow);
		index=index->next;
		if (index != NULL) {
			cprintf(verbosity,"printPath", " ->");	
		}
	}
	cprintf(verbosity,"printPath", "\n");
	if (silent == FALSE) {
		cnprintf(verbosity, "printPath", "Done");
	}
}

//if path is empty
int pathEmpty(struct Path *path) {
	return (path->city == -1 && path->next == NULL);
}

//returns # of cities
int numCities(struct Path* path) {
	int num = 0;
	if(!pathEmpty(path)) {
		struct Path *index = path;
		while(index != NULL) {
			num++;
			index = index->next;
		}
	}
	return num;
}

//checks if path contains n+1 cities
int pathFull(struct Path *path) {
	int num=numCities(path);
	int full=0;
	if(num == n+1)
		full=1;
	else
		full=0;
	return full;
}

//appends a city to path
void addCity(struct Path *path, int city)
{
	cnprintfai(FULL, "addCity", "adding", city);
	if(pathEmpty(path)) {
		path->city = city;
		path->costTillNow = 0.0;
		cnprintfai(FULL, "addCity", "Added First City", path->city);
	} else {
		struct Path *index = path;

		while(index->next != NULL) {
			cprintfai(FULL, "addCity", " ->", index->city);
			index=index->next;
		}
		cprintf(FULL, "addCity", "\n");
		//cnprintfai(FULL, "addCity", "Previous City", index->city);
		struct Path *temp = createPath();
		temp->city = city;
		temp->costTillNow = index->costTillNow + cityToCityCost(index->city, city);
		index->next = temp;
		cnprintfai(FULL, "addCity", "Added New City" ,index->next->city);
	}
	cnprintf(FULL, "addCity", "done");
}

//removes the last city from path
int removeLastCity(struct Path *path) {
	cnprintf(FULL, "removeLastCity", "start");
	if(path->city == -1 && path->next == NULL) {
		fprintf(stderr, "Error! removeLastCity got an already empty path!\n");
		cnprintf(FULL, "removeLastCity", "end");
		return -1;
	} else if (path->next == NULL) {
		int city = path->city;
		path->city = -1;
		cnprintfai(FULL, "removeLastCity", "Removed the only city", city);
		cnprintf(FULL, "removeLastCity", "end");
		return city;
	} else {
		struct Path *index = path;
		struct Path *prevIndex;
		while(index->next != NULL) {
			prevIndex = index;
			index = index->next;
		}
		prevIndex->next = NULL;
		int removedCity = index->city;
		//free memory of last city
		free(index);
		cnprintfai(FULL, "removeLastCity", "Removed a city", removedCity);
		cnprintf(FULL, "removeLastCity", "end");
		return removedCity;
	}
}

//copies a path variable. copy has different address but same element values
struct Path* copyPath(struct Path *path) {
	cnprintf(FULL, "copyPath", "start");
	struct Path *index = path;
	struct Path *copy = createPath();
	while(index != NULL) {
		addCity(copy, index->city);
		index=index->next;
	}
	cnprintf(FULL, "copyPath", "end");
	return copy;
}

//prints cost of traversing the path
double pathCost(struct Path *path) {
	struct Path *index = path;
	double cost = 0;
	while(index->next != NULL) {
		cost += *(G + (index->city) * n + index->next->city);
		index = index->next;
	}
	return cost;
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
		if (index->next == NULL) {

		}
		double cost = 0.0;
		while(index->next != NULL) {
			visited[index->city] = 1;
			cost += *(G + (index->city) * n + index->next->city);
			//this condition seems unreachable. TODO check it
			if (cost > minCost && minCost != 0.0) {
				feasible = FALSE;
				break;
			}
			index=index->next;
			//when at last node, check for visited and accumulated cost
			if (index->next == NULL) {
				visited[index->city] = 1;
				if (visited[city] == 1) {
					feasible = FALSE;
				} else {
					cost += *(G + (index->city) * n + city);
					if (cost > minCost && minCost != 0.0) {
						feasible = FALSE;
						break;
					}
				}
			}
		}
		//check for paths with just a single city
		if (path->next == NULL) {
			visited[path->city] = 1;
			if (visited[city] == 1) {
					feasible = FALSE;
			} else {
				cost += *(G + (index->city) * n + city);
				if (cost > minCost && minCost != 0.0) {
					feasible = FALSE;
				}
			}
		}
	}
	return feasible;
}

// A Linked List of paths
//a path is called a tour
struct PathsLL 
{ 
	//TODO: add pathID
	struct Path *tour;
	struct PathsLL *next;
}; 

//initializes stack. initial path is -1
struct PathsLL* createPathsLL() 
{ 
	cnprintf(FULL, "createPathsLL", "called");
	struct PathsLL* pathsLL = (struct PathsLL*) malloc(sizeof(struct PathsLL));
	pathsLL->tour = createPath();
	pathsLL->next = NULL;
	return pathsLL; 
}

int visitedCount(int visited[]) {
	int count = 0;
	for (int i=0; i<n; i++) {
		if (visited[i] == 1) {
			count += 1;
		}
	}
	return count;
}

void freePath(struct Path *path) {
	struct Path *index;
	while(path != NULL) {
		index = path;
		path = path->next;
		free(index);
	}
}

void freePathLL(struct PathsLL *pathsLL) {
	struct PathsLL *index;
	while(pathsLL != NULL) {
		index = pathsLL;
		pathsLL = pathsLL->next;
		freePath(index->tour);
		free(index);
	}
}

// returns whether pathsLL empty
int isEmpty(struct PathsLL* pathsLL) 
{ return (pathEmpty(pathsLL->tour) && pathsLL->next == NULL); } 

//adds tour to pathsLL
void push(struct PathsLL* pathsLL, struct Path* path) 
{ 
	cnprintf(FULL, "push", "start");	
	
	if(isEmpty(pathsLL)) {
		//we're replacing path into tour
		//but free old default tour first
		freePath(pathsLL->tour);
		pathsLL->tour = path;

		//next is null
		cnprintf(FULL, "push", "added the first tour pathsLL");
	}
	else { 
		struct PathsLL *index = pathsLL;
		while(index->next != NULL) {
			index = index->next;
		}
		//dont allocate for tour, only for temp
		struct PathsLL *temp = (struct PathsLL*) malloc(sizeof(struct PathsLL));// = createPathsLL();
		temp->next = NULL;
		temp->tour = path;
		index->next = temp;
		cnprintf(FULL, "push", "added a tour to pathsLL");
	}
	printPath(FULL, path, TRUE);
	cnprintf(FULL, "push", "end");
}

//removes path that was most recently added to PathsLL. outputs it
struct Path* pop(struct PathsLL* pathsLL) 
{ 
	cnprintf(FULL, "pop", "start");
	struct Path *itemPopped = pathsLL->tour;
	//if pathsLL is empty
	if (isEmpty(pathsLL)) {
		//already empty
		cnprintf(FULL, "pop", "PathsLL is already empty");
	} else if(pathsLL->next == NULL) {
		//itemPopped = pathsLL->tour;
		pathsLL->tour = createPath();
		cnprintf(FULL, "pop", "popped the only tour from PathsLL");
	} else {	 
		struct PathsLL *index = pathsLL;
		struct PathsLL *prevIndex;
		while(index->next != NULL) {
			prevIndex = index;
			index = index->next;
		}
		itemPopped = index->tour;
		prevIndex->next = NULL;
		//now free the index
		free(index);
		cnprintf(FULL, "pop", "popped a tour PathsLL");
	}
	printPath(FULL, itemPopped, TRUE);
	cnprintf(FULL, "pop", "end");
	return itemPopped;
}

//prints all the paths
void printPathsLL(int verbosity, struct PathsLL* pathsLL) {
	cnprintf(verbosity,"printPathsLL", "Printing pathsLL =>");
	cprintf(verbosity, "printPathsLL", "___________________________________\n");
	
	struct PathsLL *index = pathsLL;
	while (TRUE) {
		cprintf(verbosity, "printPathsLL", "\n");
		printPath(verbosity, index->tour, TRUE);
		if (index->next != NULL) {
			cprintf(verbosity, "printPathsLL", "\n-----------------------------------");
			index = index->next;
		} else {
			break;
		}
	}
	cprintf(verbosity, "printPathsLL", "___________________________________\n");
	cnprintf(verbosity,"printPathsLL", "end");
}

//prints distances between cities
void printAdjacencyMatrix() {
	cnprintf(LOW,"printAdjacencyMatrix", "printing Matrix");
	for (int i = 0; i<n; i++) {
		for (int j = 0; j<n; j++) {
			cprintfa(LOW,"printAdjacencyMatrix", "\t", *(G + (i*n) + j));
			if (j == n-1) {
				cprintf(LOW,"printAdjacencyMatrix", "\n");
			}
		}
	}
	cprintf(LOW,"printAdjacencyMatrix", "\n");
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

	while (!isEmpty(pathsLL)) {
		struct Path *tempPath = pop(pathsLL);
		int tempPathCityCount = numCities(tempPath);
		if ( tempPathCityCount == n) {
			//add 0th city for RTT time
			addCity(tempPath, 0);
			double tempPathCost = pathCost(tempPath);
			//saving memory
			// //adding to completePathsLL
			// push(completePathsLL, tempPath);

			if (minCost == 0) {
				minCost = tempPathCost;
				//bestPath = tempPath;
				//print only when new minCost is achieved
				printPath(LOW, tempPath, FALSE);
			} else {
				if (minCost > tempPathCost) {
					minCost = tempPathCost;

					//print only when new minCost is achieved
					printPath(LOW, tempPath, FALSE);
				}
			}
		} else {
			for (int b=n-1; b>=0; b--) {
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
	return minCost;
}

//tests whether stack works
void stackSelfTest() {
	cnprintf(LOW,"stackSelfTest", "starting");

	struct PathsLL* pathsLL = createPathsLL(); 
	struct Path *path = createPath();
	//struct Path *path2 = createPath();
	
	printf("****Adding 1 to path\n");
	addCity(path, 1);
	printPath(LOW, path, FALSE);
	printf("****Adding 2 to path\n");
	addCity(path, 2);
	printPath(LOW, path, FALSE);

	push(pathsLL, path);
	printPathsLL(LOW, pathsLL);

	printf("****Adding 3 to path\n");
	addCity(path, 3);
	//push(pathsLL, path2);
	printPathsLL(LOW, pathsLL);

	// addCity(path2, 4);
	// removeLastCity(path);
	// printPathsLL(LOW, pathsLL);
	printf("****Popping path 1->2->3\n");
	struct Path *x = pop(pathsLL);
	printPathsLL(LOW, pathsLL);

	printf("****pushing path 1->2->3\n");
	push(pathsLL, x);
	printPathsLL(LOW, pathsLL);

	printf("****Popping path 1->2->3\n");
	x = pop(pathsLL);
	printPath(LOW, x, FALSE);
	printPathsLL(LOW, pathsLL);
	
	printf("****pushing path 1->2->3\n");
	push(pathsLL, x);
	printPathsLL(LOW, pathsLL);
	
	freePathLL(pathsLL);
	// printPathsLL(LOW, pathsLL);
	// addCity(path, 20);
	// push(pathsLL, path);
	// printPathsLL(LOW, pathsLL);
	// addCity(path, 30);
	// push(pathsLL, path);
	// printPathsLL(LOW, pathsLL);
	// addCity(path, 40);
	// push(pathsLL, path);
	// printPathsLL(LOW, pathsLL);
	// pop(pathsLL);
	// printPathsLL(LOW, pathsLL);
	// pop(pathsLL);
	// printPathsLL(LOW, pathsLL);
	// pop(pathsLL);
	// printPathsLL(LOW, pathsLL);
	// pop(pathsLL);
	// printPathsLL(LOW, pathsLL);
	// pop(pathsLL);
	// printPathsLL(LOW, pathsLL);
	// printPath(LOW, path, FALSE);
	// removeLastCity(path);
	// removeLastCity(path);
	// removeLastCity(path);
	// removeLastCity(path);
	// removeLastCity(path);
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
	printf("file_name:%s\n", file_name);
	printf("n:%d\n", n);

	clock_t startTime, endTime;
    double cpu_time_used;
	
	G = (double*) malloc(n * n * sizeof(double)); 


	// // 4 cities case
	// // G[0][1] = 20.0;
	// // G[0][2] = 42.0;
	// // G[0][3] = 35.0;

	// // G[1][2] = 30.0;
	// // G[1][3] = 34.0;
	
	// // G[2][3] = 12.0;

	// // 9 cities case
	// G[0][1] = 5.0;
	// G[0][4] = 10.0;
	// G[0][5] = 15.0;

	// G[1][2] = 25.0;
	// G[1][3] = 5.0;

	// G[2][8] = 10.0;

	// G[3][8] = 15.0;
	// G[3][7] = 35.0;

	// G[4][5] = 5.0;
	// G[4][6] = 15.0;

	// G[5][8] = 25.0;
	// G[5][7] = 15.0;
	// G[5][6] = 10.0;

	// G[6][7] = 20.0;

	// G[7][8] = 30.0;

	// //replicate across the diagonal
	// for (int i = 0; i<n; i++) {
	// 	for (int j = i; j<n; j++) {
	// 		G[j][i] = G[i][j];
	// 	}
	// }
    char ch;
   	FILE *fp;
    fp = fopen(file_name, "r"); // read mode
    if (fp == NULL) {
		printf("Error while opening the file.\n");
		exit(0);
   	}

   	double numberArray[n*n];
   	for (int i = 0; i < n*n; i++){
        fscanf(fp, "%lf", &numberArray[i] );
    }
   	fclose(fp);
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

    printf("\n\n\n");
	printf("=====================================\n");
	printf("Lowest Cost:%.2f\n", minCost);
	//printf("There were %d possible paths.\n", competingPaths);
	printf("\nTook %.10f seconds to execute\n", cpu_time_used);

	free(G);
	return 0; 
} 
