// C program for array implementation of stack 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <time.h>

//verbosity levels
#define LOW 1
#define MEDIUM 2
#define FULL 3
#define CVERBOSE LOW
#define cnprintf(lvl,caller,str) ((CVERBOSE>=lvl) ? printf("%s: %s\n",caller,str) : 0);
#define cnprintfa(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? printf("%s: %s = %.2f\n",caller,str,arg) : 0);
#define cnprintfaa(lvl,caller,str,arg1,arg2) ((CVERBOSE>=lvl) ? printf("%s: %s = %.2f:%.2f\n",caller,str,arg1,arg2) : 0);
#define cprintf(lvl,caller,str) ((CVERBOSE>=lvl) ? printf("%s",str) : 0);
#define cprintfa(lvl,caller,str,arg) ((CVERBOSE>=lvl) ? printf("%s %.2f",str,arg) : 0);

// A structure to represent a stack 
struct Stack 
{ 
	int top; 
	unsigned capacity; 
	double* array; 
}; 

// function to create a stack of given capacity. It initializes size of 
// stack as 0 
struct Stack* createStack(unsigned capacity) 
{ 
	struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack)); 
	stack->capacity = capacity; 
	stack->top = -1; 
	stack->array = (double*) malloc(stack->capacity * sizeof(double)); 
	return stack; 
} 

// Stack is full when top is equal to the last index 
int isFull(struct Stack* stack) 
{ return stack->top == stack->capacity - 1; } 

// Stack is empty when top is equal to -1 
int isEmpty(struct Stack* stack) 
{ return stack->top == -1; } 

// Function to add an item to stack. It increases top by 1 
void push(struct Stack* stack, int item) 
{ 
	if (isFull(stack)) 
		return; 
	stack->array[++stack->top] = item; 
	cnprintfa(FULL, "push", "pushed to stack", item); 
} 

// Function to remove an item from stack. It decreases top by 1 
int pop(struct Stack* stack) 
{ 
	if (isEmpty(stack)) 
		return INT_MIN; 
	return stack->array[stack->top--]; 
} 

void printStack(int verbosity, struct Stack* stack) {
	cprintf(verbosity,"printStack", "Printing stack => ");
	
	int count = stack->top + 1;
	if (count == 0) {
		cprintf(verbosity,"printStack", " (empty)\n");
	}
	for (int i = 0; i < count; i++) {
		cprintfa(verbosity,"printStack", "", stack->array[i]);
		if (i != count -1) {
			cprintf(verbosity,"printStack", " -> ");
		} else {
			if (i == stack->capacity - 1) {
				cprintf(verbosity,"printStack", " (full)");
			}
			cprintf(verbosity,"printStack", "\n");
		}
	}
	
}

void stackSelfTest() {
	cnprintf(LOW,"stackSelfTest", "starting");
	struct Stack* stack = createStack(3); 
	printStack(LOW,stack);
	push(stack, 10); 
	printStack(LOW,stack);
	push(stack, 20); 
	printStack(LOW,stack);
	push(stack, 30); 
	printStack(LOW,stack);

	cnprintfa(LOW,"stackSelfTest", "popped from stack", pop(stack)); 
	printStack(LOW,stack);
}

double G[9][9];
int n;    //n is no of cities
double minCost = 0.0;
int competingPaths = 0;

void printAdjacencyMatrix() {
	cnprintf(LOW,"printAdjacencyMatrix", "printing Matrix");
	for (int i = 0; i<n; i++) {
		for (int j = 0; j<n; j++) {
			cprintfa(LOW,"printAdjacencyMatrix", "\t", G[i][j]);
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

void DFS(int curStartPoint, double costTillNow, int firstPoint, struct Stack* stack, int visited[]) {
	printStack(FULL, stack);
	cnprintfaa(MEDIUM, "DFS", "\tcurStartPoint:costTillNow = ", curStartPoint, costTillNow);
	double accumulatedCost = 0;
	accumulatedCost += costTillNow;
	
	//printf("%d ->", curStartPoint);
	push(stack,curStartPoint);
	if (curStartPoint == firstPoint) {
		visited[curStartPoint] = 1;
	}
	
	for (int j=0; j<n; j++) {
		//check if path exists
		if (G[curStartPoint][j] != 0.0) {
			//if visited, check if we reached back
			if (visited[j] == 1) {
				if (j == firstPoint && visitedCount(visited) == n) {
					//time to break
					double newCost = accumulatedCost + G[curStartPoint][j];
					if (minCost > newCost || minCost == 0.0) {
						minCost = newCost;
						//TODO save stack
					}
					competingPaths += 1;
					cnprintfa(LOW, "DFS", "Final cost:", accumulatedCost + G[curStartPoint][j]);
					printStack(LOW, stack);
					cnprintf(LOW, "DFS", "---------------------------------------------------------------");
				} else if (j == firstPoint) {
					//incomplete solution
					//skip the solution which reaches firstPoint without 
					//going through all other points
					continue;
				} else {
					//wrong direction
					//we have already visited this non-firstPoint
					continue;
				}
			} else {
				visited[j] = 1;
				DFS(j, accumulatedCost + G[curStartPoint][j], firstPoint, stack, visited);
				//restore visits
				visited[j] = 0;
				pop(stack);
			}
		}
	}	
}

// Driver program to test above functions 
int main() 
{ 
	clock_t startTime, endTime;
    double cpu_time_used;
	//stackSelfTest();
	int cities = 9;
	n = cities;
	int visited[n];

	//init
	struct Stack* stack = createStack(n);
	for (int i = 0; i<n; i++) {
		for (int j = 0; j<n; j++) {
			G[i][j] = 0.0;
		}
		visited[i] = 0;
	}

	// 4 cities case
	// G[0][1] = 20.0;
	// G[0][2] = 42.0;
	// G[0][3] = 35.0;

	// G[1][2] = 30.0;
	// G[1][3] = 34.0;
	
	// G[2][3] = 12.0;

	// 9 cities case
	G[0][1] = 5.0;
	G[0][4] = 10.0;
	G[0][5] = 15.0;

	G[1][2] = 25.0;
	G[1][3] = 5.0;

	G[2][8] = 10.0;

	G[3][8] = 15.0;
	G[3][7] = 35.0;

	G[4][5] = 5.0;
	G[4][6] = 15.0;

	G[5][8] = 25.0;
	G[5][7] = 15.0;
	G[5][6] = 10.0;

	G[6][7] = 20.0;

	G[7][8] = 30.0;

	//replicate across the diagonal
	for (int i = 0; i<n; i++) {
		for (int j = i; j<n; j++) {
			G[j][i] = G[i][j];
		}
	}

	printAdjacencyMatrix();

	int start = 0;
	
	startTime = clock();
	
	DFS(start, 0, start, stack, visited);
	
	endTime = clock();
    cpu_time_used = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;

	printf("=====================================\n");
	printf("Lowest Cost:%.2f\n", minCost);
	printf("There were %d possible paths.\n", competingPaths);
	printf("\nTook %.6f seconds to execute\n", cpu_time_used);


	return 0; 
} 
