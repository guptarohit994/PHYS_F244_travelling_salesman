// C program for array implementation of stack 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
#define CVERBOSE 1
#define cnprintf(caller,str) ((CVERBOSE==1) ? printf("%s: %s\n",caller,str) : 0);
#define cnprintfa(caller,str,arg) ((CVERBOSE==1) ? printf("%s: %s = %d\n",caller,str,arg) : 0);
#define cprintf(caller,str) ((CVERBOSE==1) ? printf("%s",str) : 0);
#define cprintfa(caller,str,arg) ((CVERBOSE==1) ? printf("%s %d",str,arg) : 0);

// A structure to represent a stack 
struct Stack 
{ 
	int top; 
	unsigned capacity; 
	int* array; 
}; 

// function to create a stack of given capacity. It initializes size of 
// stack as 0 
struct Stack* createStack(unsigned capacity) 
{ 
	struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack)); 
	stack->capacity = capacity; 
	stack->top = -1; 
	stack->array = (int*) malloc(stack->capacity * sizeof(int)); 
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
	cnprintfa("push", "pushed to stack", item); 
} 

// Function to remove an item from stack. It decreases top by 1 
int pop(struct Stack* stack) 
{ 
	if (isEmpty(stack)) 
		return INT_MIN; 
	return stack->array[stack->top--]; 
} 

void printStack(struct Stack* stack) {
	cprintf("printStack", "Printing stack => ");
	
	int count = stack->top + 1;
	if (count == 0) {
		cprintf("printStack", "(empty)\n");
	}
	for (int i = 0; i < count; i++) {
		cprintfa("printStack", "", stack->array[i]);
		if (i != count -1) {
			cprintf("printStack", "-> ");
		} else {
			if (i == stack->capacity - 1) {
				cprintf("printStack", "(full)");
			}
			cprintf("printStack", "\n");
		}
	}
	
}

void stackSelfTest() {
	cnprintf("stackSelfTest", "starting");
	struct Stack* stack = createStack(3); 
	printStack(stack);
	push(stack, 10); 
	printStack(stack);
	push(stack, 20); 
	printStack(stack);
	push(stack, 30); 
	printStack(stack);

	cnprintfa("stackSelfTest", "popped from stack", pop(stack)); 
	printStack(stack);
}

int G[4][4],n;    //n is no of vertices and graph is sorted in array G[10][10]

void printAdjacencyMatrix() {
	cnprintf("printGraph", "printing");
	for (int i = 0; i<n; i++) {
		for (int j = 0; j<n; j++) {
			cprintfa("printGraph", "", G[i][j]);
			if (j == n-1) {
				cprintf("printGraph", "\n");
			}
		}
	}
	cprintf("printGraph", "\n");
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

void DFS(int curStartPoint, int costTillNow, int firstPoint, struct Stack* stack, int visited[]) {
	printStack(stack);
	printf("DFS:\tcurStartPoint:%d, costTillNow:%d\n", curStartPoint, costTillNow);
	int accumulatedCost = 0;
	accumulatedCost += costTillNow;
	
	//printf("%d ->", curStartPoint);
	push(stack,curStartPoint);
	if (curStartPoint == firstPoint) {
		visited[curStartPoint] = 1;
	}
	
	for (int j=0; j<n; j++) {
		//check if path exists
		if (G[curStartPoint][j] != 0) {
			//if visited, check if we reached back
			if (visited[j] == 1) {
				if (j == firstPoint && visitedCount(visited) == n) {
					//time to break
					printf("Final cost:%d\n", accumulatedCost + G[curStartPoint][j]);
					printStack(stack);
					printf("---------------------------------------------------------------\n");
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
	//stackSelfTest();
	int cities = 4;
	n = cities;
	int visited[n];

	//init
	struct Stack* stack = createStack(n);
	for (int i = 0; i<n; i++) {
		for (int j = 0; j<n; j++) {
			G[i][j] = 0;
		}
		visited[i] = 0;
	}

	G[0][1] = 20;
	G[1][0] = 20;


	G[0][2] = 42;
	G[2][0] = 42;


	G[0][3] = 35;
	G[3][0] = 35;


	G[1][2] = 30;
	G[2][1] = 30;


	G[1][3] = 34;
	G[3][1] = 34;


	G[2][3] = 12;
	G[3][2] = 12;

	printAdjacencyMatrix();

	int start = 0;
	DFS(start, 0, start, stack, visited);


	return 0; 
} 
