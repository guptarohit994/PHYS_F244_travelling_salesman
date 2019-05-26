#ifndef DEPTHFIRST_H
#define DEPTHFIRST_H

#define _POSIX_SOURCE 1
#define _BSD_SOURCE
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <time.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

//verbosity levels
#define LOW 1
#define MEDIUM 2
#define FULL 3
#define CVERBOSE FULL
#define cnprintf(lvl,caller,str) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s: %s\n",caller,str) : 0);
#define cnprintfa(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s: %s = %.2lf\n",caller,str,arg) : 0);
//custom print with one argument (integer)
#define cnprintfai(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s: %s = %d\n",caller,str,arg) : 0);
//custom print with two arguments (double)
#define cnprintfaa(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s: %s = %.2lf:%.2lf\n",caller,str,arg1,arg2) : 0);
//custom print with first argument (integer) and second argument double
#define cnprintfaia(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s: %s = %d:%.2lf\n",caller,str,arg1,arg2) : 0);

#define cprintf(lvl,caller,str) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s",str) : 0);

#define cprintfa(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s %.2lf",str,arg) : 0);

#define cprintfaia(lvl,caller,str,arg1, arg2) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s %d(%.2lf)",str,arg1, arg2) : 0);
//custom print with an argument (integer)
#define cprintfai(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s %d",str,arg) : 0);
//custom print with two arguments (integer)
#define cnprintfaai(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? fprintf(outfile_fp, "%s: %s = %d:%d\n",caller,str,arg1,arg2) : 0);

#define TRUE 1
#define FALSE 0

double* G;
int n;    //n is no of cities
double minCost = 0.0;
int competingPaths = 0;
char file_name[100];
char outfile_name[100];
FILE *outfile_fp;

int handle_option(char *arg);

void usage();

//update the size of FILE IO buffers
void setBufSize(int numCities) {
	setlinebuf(outfile_fp);
	// struct stat stats;
 //    if(fstat(fileno(outfile_fp), &stats) == -1) { // POSIX only
 //        perror("fstat");
 //    }
 
 //    printf("BUFSIZ is %d, but optimal block size is %ld\n", BUFSIZ, stats.st_blksize);
 //    if(setvbuf(outfile_fp, NULL, _IOLBF, stats.st_blksize) != 0) {
 //       perror("setvbuf failed"); // POSIX version sets errno
 //    }
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

/*
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
*/
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
/*
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
*/

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
/*
int visitedCount(int visited[]) {
	int count = 0;
	for (int i=0; i<n; i++) {
		if (visited[i] == 1) {
			count += 1;
		}
	}
	return count;
}
*/
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
    pathsLL->tour = createPath(); //so that itemPopped is copy of stack, and not same address
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

int numPaths(struct PathsLL* pathsLL) {
	int num = 0;
	if (!isEmpty(pathsLL)) {
		while (pathsLL != NULL) {
			num++;
			pathsLL = pathsLL->next;
		}
	}
	return num;

}

struct PathsLL* get_my_share(struct PathsLL *pathsLL, int threadID, int pathsPerThread) {
	cnprintfai(LOW, "get_my_share", "Starting from threadID", threadID);
	int numPathsAvailable = numPaths(pathsLL);

	cnprintfaai(LOW, "get_my_share", "threadID:numPathsAvailable", threadID, numPathsAvailable);
	cnprintfaai(LOW, "get_my_share", "threadID:pathsPerThread", threadID, pathsPerThread);
	
	if (pathsPerThread > numPathsAvailable) {
		pathsPerThread = numPathsAvailable;
	}
	cnprintfaai(LOW, "get_my_share", "threadID:pathsAllocated", threadID, pathsPerThread);
	struct PathsLL *newPathsLL = createPathsLL();
	for (int i=0; i<pathsPerThread; i++) {
			push(newPathsLL, pop(pathsLL));
	}
	cnprintfai(LOW, "get_my_share", "Ending from threadID", threadID);
	return newPathsLL;
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

  pop(pathsLL);
  printPathsLL(LOW, pathsLL);
  pop(pathsLL);
  printPathsLL(LOW, pathsLL);
  pop(pathsLL);
  printPathsLL(LOW, pathsLL);
  pop(pathsLL);
  printPathsLL(LOW, pathsLL);

	freePathLL(pathsLL);
}

#endif
