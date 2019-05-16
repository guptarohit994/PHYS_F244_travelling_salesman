#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 5

typedef enum { false, true } boolean;

static const int distance[N][N]={{-1, 3, 5, 2, 8}, {3.1, -1, 9, 4.2, 6.4}, {5.1, 9.1, -1, 6.2, 6.3}, {1.3, 1.5, 7.5, 9.3, 11}};	//can't replace with N?

//Linked list code from https://www.hackerearth.com/practice/data-structures/linked-list/singly-linked-list/tutorial/
struct LinkedList{
    int data;
    struct LinkedList *next;
 };

typedef struct LinkedList *node; //Define node as pointer of data type struct LinkedList

node createNode(){
    node temp; // declare a node
    temp = (node)malloc(sizeof(struct LinkedList)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}

node addNode(node head, int value){
    node temp,p;// declare two nodes temp and p
    temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
    temp->data = value; // add element's value to data part of node
    if(head == NULL){
        head = temp;     //when linked list is empty
    }
    else{
        p  = head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return head;
}

int nodeSize(node path) {
	int size=1;
	node temp=path;
	while(temp != NULL) {
	temp=temp->next;
	size+=1;
	}
	return size;
}

void printNode(node p) {
	while(p != NULL) {
	printf("%d\n", p->data);
	p=p->next;
	}
}

int pathCost(node path) {
	node p;
	p=path;
	p=p->next;
	int sum=0;
	int tempsum;
	while(p != NULL) {
		tempsum=distance[p->data][(p->next)->data];
		sum+=tempsum;
		p=p->next;
	}
	return sum;
}

struct ListList{
    node data;
    struct ListList *next;
 };

typedef struct ListList *nodeList; //Define node as pointer of data type struct LinkedList

nodeList createNodeList(){
    nodeList temp; // declare a node
    temp = (nodeList)malloc(sizeof(struct ListList)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
	node tempNode;
	tempNode=createNode();
	tempNode->data=-1;
	temp->data=tempNode;
    return temp;//return the new node
}

nodeList push(nodeList head, node path){
    nodeList temp,p;// declare two nodes temp and p
    temp = createNodeList();//createNode will return a new node with data = value and next pointing to NULL.
    temp->data = path; // add element's value to data part of node
    if(head == NULL){
        head = temp;     //when linked list is empty
	printf("headnull\n");
	//printStack(head);
    }
    else{
        p  = head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return head;
}

//if head is only 1 item does nothing
node pop(nodeList head){
    nodeList temp,p;// declare two nodes temp and p
	node tempNode;
    temp = createNodeList();//createNode will return a new node with data = value and next pointing to NULL.
    if(head == NULL){
        tempNode = NULL;     //when linked list is empty
	printf("headnull\n");
	//printStack(head);
    }
    else{
        p  = head;//assign head to p 
        while(p->next != NULL){
		temp=p;
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
     	tempNode=p->data;
	temp->next=NULL;
	
    }
    return tempNode;
}


void printStack(nodeList path) {
	nodeList p;
	p=path;
	p=p->next;
	while(p != NULL) {
	printNode(p->data);
	printf("path end\n");
	p=p->next;
	}
	printf("\n");
}

//makes sure not going to the city you are already at
boolean feasible(node path, int city) {
	node p=path;
	while(p->next != NULL) {
		p=p->next;
	}
	int last = p->data;
	if(last != city)
		return true;
	else
		return false;
}

int main(int argc, char *argv[])
{
	//const int N=5;
	const int start=0;
	//static const int distance[N][N]={{-1, 3, 5, 2, 8}, {3.1, -1, 9, 4.2, 6.4}, {5.1, 9.1, -1, 6.2, 6.3}, {1.3, 1.5, 7.5, 9.3, 11}};	//can't replace with N?
	nodeList stack = createNodeList();
/*
	stack=NULL;
	printf("test\n");
	if(stack==NULL) {
		printf("null\n");
	}
	printf("afternull\n");
*/
	//(stack->data)->data=distance[start][start+1];
	node temp;
	int i,j;

	for(i=0; i<N; i++) {	//figure out how to check initialization condition
		if(i != start) {
		temp = createNode();
		//temp->data = distance[start][i];
		temp->data=i;
		push(stack, temp);
		}
	}
	printStack(stack);


	boolean done=false;
	int popdone=0;
	int minDist=9999;
	node bestPath=createNode();
	#pragma omp parallel shared(stack, minDist, bestPath, done, popdone)
	{
		while(!done) {
			printStack(stack);
			node tempPath;
			#pragma omp critical
			{
				tempPath=pop(stack);
			}
			popdone+=1;
			if((tempPath->data)==-1) {
				if(popdone==1)
					done=true;
				else
					popdone-=1;
			}
			else if(nodeSize(tempPath)>=N) {
				int tempCost=pathCost(tempPath);
				#pragma omp critical
				{
					if(tempCost<minDist) {
						minDist=tempCost;
						bestPath=tempPath;
					}
				}
				popdone-=1;
			}
			else {
				int i;
				node newPath;
				for (i=0; i<N; i++)
				{
					if(feasible(tempPath, i)) {
					newPath=addNode(tempPath, i);
						#pragma omp critical
						{
							push(stack, newPath);
						}
					}
				}
				popdone-=1;
			}

		}
	}
}
