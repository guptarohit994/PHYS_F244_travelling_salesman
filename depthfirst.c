#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

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

void printNode(node p) {
	while(p != NULL) {
	printf("%d\n", p->data);
	p=p->next;
	}
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

void printStack(nodeList p) {
	p=p->next;
	while(p != NULL) {
	printNode(p->data);
	printf("path end\n");
	p=p->next;
	}
}

int main(int argc, char *argv[])
{
	const int N=5;
	const int start=0;
	const int distance[5][5]={{-1, 3, 5, 2, 8}, {3.1, -1, 9, 4.2, 6.4}, {5.1, 9.1, -1, 6.2, 6.3}, {1.3, 1.5, 7.5, 9.3, 11}};	//can't replace with N?
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
		temp->data = distance[start][i];
		push(stack, temp);
		}
	}
	printStack(stack);
/*

  #pragma omp parallel for shared(N, start, distance, stack) private(minDist, bestPath)
	{
		int i;
		for (i=0; i<N; i++)
		{

		}
	}
*/
}
