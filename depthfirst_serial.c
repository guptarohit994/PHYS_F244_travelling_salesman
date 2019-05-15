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
#define CVERBOSE FULL
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
//custom print with an argument (integer)
#define cprintfai(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? printf("%s %d",str,arg) : 0);
//custom print with two arguments (integer)
#define cnprintfaai(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? printf("%s: %s = %d:%d\n",caller,str,arg1,arg2) : 0);

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

//Linked List of path taken
struct Path
{
	int city;
	struct Path *next;
};

//initializes city to -1. empty path is when city is -1
struct Path* createPath()
{
	struct Path* path = (struct Path*) malloc(sizeof(struct Path));
	path->city=-1;
	path->next=NULL;
	return path;
}

//prints path if CVERBOSE >= verbosity
void printPath(int verbosity, struct Path* path)
{
	cprintf(verbosity, "printPath", "\n");
	cprintf(verbosity,"printPath", "Printing path => \n");
	struct Path *index=path;
	while(index != NULL) {
		cprintfai(verbosity,"printPath", "", index->city);
		if(index->next != NULL)
			cprintf(verbosity,"printPath", " -> ");	
		index=index->next;
	}
	cprintf(verbosity,"printPath", "\n");
	cprintf(verbosity, "printPath", "\n");
}

//if path is empty
int pathEmpty(struct Path *path) {
	return path->city==-1;
}

//returns # of cities
int numCities(struct Path* path) {
	int num = 0;
	if(pathEmpty(path))
		;
	else {
		struct Path *index=path;
		while(index != NULL) {
			num++;
			index=index->next;
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
void addCity(struct Path *path, int place)
{
	cprintf(FULL, "addCity", "\n");
	if(path->city == -1) {
		path->city = place;
		cnprintfai(FULL, "addCity", "Path", path->city);
	}
	else {
		cnprintf(FULL,"addCity", "");
		struct Path *index = path;
		while(index->next != NULL) {
			cprintfai(FULL, "addCity", "->", index->city);
			index=index->next;
		}
		cprintfai(FULL, "addCity", "->", index->city);
		struct Path *temp = createPath();
		temp->city=place;
		index->next=temp;
		cprintfai(FULL, "addCity", "->" ,index->next->city);
		cprintf(FULL, "addCity", "\n");
	}
	cprintf(FULL, "addCity", "\n");
}

//removes the last city from path
void removeLastCity(struct Path *path) {
	cprintf(FULL, "removeLastCity", "\n");
	cnprintf(FULL, "removeLastCity", "start");
	if(path->city == -1)
		;
	else {
		struct Path *index = path;
		while(index->next != NULL) {
			index=index->next;
		}
		index->next=NULL;
	}
	printPath(FULL, path);
	cnprintf(FULL, "removeLastCity", "end\n");
}

//copies a path variable. copy has different address but same element values
struct Path* copyPath(struct Path *path) {
	cprintf(FULL, "copyPath", "\n");
	cnprintf(FULL, "copyPath", "start");
	struct Path *index = path;
	struct Path *copy = createPath();
	while(index != NULL) {
		addCity(copy, index->city);
		index=index->next;
	}
	cnprintf(FULL, "copyPath", "end");
	cprintf(FULL, "copyPath", "\n");
	return copy;
}

//prints cost of traversing the path
double cost(struct Path *path) {
	struct Path *index = path;
	double cost = 0;
	while(index->next != NULL) {
		cost += *(G + (index->city)*n + index->next->city);
		index=index->next;
	}
	return cost;
}


//returns whether path is feasible. ie: whether went to same city twice (unless is starting city and path is complete),
//whether path is empty, whether cost of path less than minCost of complete paths (perhaps do this in DFS instead), etc.
int feasible(struct Path *path) {
/*
	int feasible=1;

	int visited[n];
	//init
	for (int i = 0; i<n; i++) {
		visited[i] = 0;
	}
	if(pathEmpty(path))
		feasible = 0;
	else if {
		struct Path *index = path;
		while(index != NULL) {
			if(visited[index->city]==1) {
				if(index->city==path->city && numCities==n)
					;
				else
					feasible = 0;
			}
			visited[index->city]=1;
			index=index->next;
		}
	}
*/
	return 1;
}

// A Linked List of paths
struct Stack 
{ 
	int top; 
	//unsigned capacity; 
	//int* array; 
	struct Path *array;
	struct Stack *next;
}; 

//initializes stack. initial path is -1
struct Stack* createStack() 
{ 
	struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));
	//stack->capacity = capacity; 
	stack->top = -1; 
	//stack->array = (int*) malloc(stack->capacity * sizeof(int)); 
	stack->array=createPath();
	stack->next=NULL;
	return stack; 
}
/*
// Stack is full when top is equal to the last index 
int isFull(struct Stack* stack) 
{ return stack->top == stack->capacity - 1; } 
*/
// returns whether stack empty
int isEmpty(struct Stack* stack) 
{ return stack->array->city == -1; } 

//adds path to stack
void push(struct Stack* stack, struct Path* path) 
{ 
	cprintf(FULL, "push", "\n");
	cnprintf(FULL, "push", "start");	
	struct Path* copy = copyPath(path);
	if(isEmpty(stack)) {
		stack->array=copy;
	}
	else { 
		struct Stack *index=stack;
		while(index->next != NULL) {
			index=index->next;
		}
		struct Stack *temp=createStack();
		temp->array=copy;
		index->next=temp;
	}
	cnprintf(FULL, "push", "pushed to stack");
	printPath(FULL, copy);
	cnprintf(FULL, "push", "end\n");
}

//removes path that was most recently added to stack. outputs it
struct Path* pop(struct Stack* stack) 
{ 
	cprintf(FULL, "pop", "\n");
	cnprintf(FULL, "pop", "start");
	struct Path *path;
	if(stack->next == NULL) {
		path=stack->array;
		stack->array = createPath();
	}
	else {	 
		struct Stack *index = stack;
		while((index->next)->next != NULL) {
			index=index->next;
		}
		struct Stack *temp=index->next;
		path = temp->array;
		//TODO: flush temp from memory
		index->next=NULL;
	}
	cnprintf(FULL, "pop", "popped from stack");
	printPath(FULL, path);
	cprintf(FULL, "pop", "pop end\n\n");
	return path;
}

//prints all the paths
void printStack(int verbosity, struct Stack* stack) {
	cprintf(verbosity, "printstack", "\n");
	cprintf(verbosity,"printStack", "Printing stack => \n");
	struct Stack *index=stack;
	while(index != NULL) {
		struct Path *temp = index->array;
		printPath(verbosity, temp);
		cnprintf(verbosity, "printStack", "Path end");
		index=index->next;
	}
	cprintf(verbosity, "printStack", "-----Printing Stack end-----\n\n");
}

//tests whether stack works
void stackSelfTest() {
	cnprintf(LOW,"stackSelfTest", "starting");
	struct Stack* stack = createStack(); 
	struct Path *path = createPath();
	addCity(path, 10);
	push(stack, path);
	printStack(LOW, stack);
	addCity(path, 20);
	push(stack, path);
	printStack(LOW, stack);
	addCity(path, 30);
	push(stack, path);
	printStack(LOW, stack);
	addCity(path, 40);
	push(stack, path);
	printStack(LOW, stack);
	pop(stack);
	printStack(LOW, stack);
	pop(stack);
	printStack(LOW, stack);
	pop(stack);
	printStack(LOW, stack);
	pop(stack);
	printStack(LOW, stack);
	pop(stack);
	printStack(LOW, stack);
	printPath(LOW, path);
	removeLastCity(path);
	removeLastCity(path);
	removeLastCity(path);
	removeLastCity(path);
	removeLastCity(path);
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

int visitedCount(int visited[]) {
	int count = 0;
	for (int i=0; i<n; i++) {
		if (visited[i] == 1) {
			count += 1;
		}
	}
	return count;
}

//main algorithm. prints minCost and the best path
void DFS(double costTillNow, int firstPoint, int visited[]) {
	struct Stack* stack = createStack();
	struct Path* path = createPath();
	addCity(path, firstPoint);
	push(stack, path);
	double minCost=0;
	while(!isEmpty(stack)) {
		printStack(FULL, stack);
		path = pop(stack);
		cnprintfai(MEDIUM, "DFS", "\tcostTillNow = ", cost(path));
		for(int i = 0; i<n; i++) {
			addCity(path, i);
			if(feasible(path)) {
				if(pathFull(path)) {
					minCost=cost(path);
					cnprintfa(LOW, "DFS", "Final cost:", minCost);
					printStack(LOW, stack);
					cnprintf(LOW, "DFS", "---------------------------------------------------------------");

				}
				else {
					push(stack, path);
				}
			}
			removeLastCity(path);
		}
	}
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
	
	int visited[n];

	//init
	struct Stack* stack = createStack();
	for (int i = 0; i<n; i++) {
		for (int j = 0; j<n; j++) {
			*(G + (i*n) + j) = 0.0;
		}
		visited[i] = 0;
	}

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
	
	//DFS(start, 0, start, stack, visited);
	stackSelfTest();	

	endTime = clock();
    cpu_time_used = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;

	printf("=====================================\n");
	printf("Lowest Cost:%.2f\n", minCost);
	printf("There were %d possible paths.\n", competingPaths);
	printf("\nTook %.10f seconds to execute\n", cpu_time_used);


	return 0; 
} 
