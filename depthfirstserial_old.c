#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define N 4

typedef enum { false, true } boolean;

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

void printNode(node p) {
	if(p==NULL)
		printf("Null\n");
	else {
		while(p != NULL) {
		printf("City %d\n", p->data);
		p=p->next;
		}
	}
}

node addNodeChange(node head, int value){
	printf("addNodeChange start\n");
    node temp;
    temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
    temp->data = value; // add element's value to data part of node

    if(head == NULL){
        head = temp;     //when linked list is empty
    }
    else{
        node p  = head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return head;
}

node duplicate(node head){
	printf("duplicate start\n");
	node out;
	out=NULL;
	node p=head;
	while(p != NULL){	//check for when p=null
		printf("duplicate while\n");

		int tempData=p->data;
		out=addNodeChange(out,tempData);
		p=p->next;
	}
	printf("duplicate end\n");
	return out;
}

node addNode(node head, int value){
    node temp,p;// declare two nodes temp and p
    temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
    temp->data = value; // add element's value to data part of node
	node out=duplicate(head);
    if(head == NULL){
        out = temp;     //when linked list is empty
    }
    else{
        p  = out;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return out;
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

double pathCost(node path, double distance[N][N]) {//path must have at least 2
	node p;
	p=path;
	//p=p->next;
	double sum=0;
	double tempsum;
	while(p->next != NULL) {
		tempsum=distance[p->data][(p->next)->data];
	printf("p->data is %d\n", p->data);
	printf("(p->next)->data is %d\n", (p->next)->data);
	printf("distance[3][2]=%d\n", distance[3][2]);
	printf("tempsum is %d\n", tempsum);
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
    nodeList p;// declare two nodes temp and p
	node tempNode;
    if(head == NULL){
        tempNode = NULL;     //when linked list is empty
	printf("headnull\n");
	//printStack(head);
    }
    else{
        p  = head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
     	tempNode=p->data;
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
	double distance[N][N]={{-1.3, 3.1, 2.1, 8.1}, {3.1, -1.1, 4.2, 6.4}, {5.1, -1.1, 6.2, 6.3}, {1.3, 7.5, 9.3, 11.1}};
	//distance[1][1]
	printf("%f\n", distance[1][2]);
	const int start=0;
	nodeList stack = createNodeList();
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

	temp=pop(stack);
	printf("pop is ");
	printNode(temp);
	temp=addNodeChange(temp, 2);
	printf("addNodechange is ");
	printNode(temp);
	node newTemp=duplicate(temp);
	newTemp=addNodeChange(newTemp, 1);
	printf("temp is");
	printNode(temp);
	printf("newTemp is ");
	printNode(newTemp);
	printf("Cost is %f\n", pathCost(temp, distance));
	
	
	boolean done=false;
	int popdone=0;
	int minDist=9999;
	node bestPath=createNode();
		while(!done) {
			printStack(stack);
			node tempPath=pop(stack);
			if((tempPath->data)==-1) {
				done=true;
			}
			else if(nodeSize(tempPath)>=N) {
				int tempCost=pathCost(tempPath, distance);
					if(tempCost<minDist) {
						minDist=tempCost;
						bestPath=tempPath;
					}
			}
			else {
				int i;
				node newPath;
				for (i=0; i<N; i++)
				{
					if(feasible(tempPath, i)) {
						newPath=addNode(tempPath, i);
						push(stack, newPath);
					}
				}
			}

		}
	
	
}
