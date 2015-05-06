/*----------------------------------------------

Name : Kapil Mahaveer Somani
Unity ID : kmsomani

CSC 520 - Artificial Intelligence I ( FALL 2014 )
Assignment I
09-07-2014
SearchRomania.c

Implements DFS and DFS algorithms on the 
	Romania map to find routes between
	various cities.

Map : SearchRomani.png
-----------------------------------------------*/

//Include necessary libraries
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*Node structure which will be used to implement
	stack and queue operations required for
	DFS and BFS traversals
*/
typedef struct node
{
	int data;
	struct node *next;
}node;

//Function declarations
void initAdjMat();	//Initializes the Adjacency Matrix
void printAdjMat();	//Prints the Adjacency Matrix
void addRoute(int,int);	//Add routes between different cities
//void findRoute(char*,char*);
void map();	//Maps various routes, uses addRoute(int,int)
void DFS(int,int);	//Implements DFS algorithm
void push(int);		//function of stack
int pop();		//function of stack
void BFS();		//Imnplements BFS algoritm
void addQ(int);		//function of queue
int delQ();		//function of queue
int emptyQ();		//function of queue
int getCityId(char *);	//get unique Id for each city

int visited[26];	//stores wether a particular city is visited
int adjMat[26][26];	//Adjacency Matrix
node *head;		//pointer for stack
node *front,*rear;	//pointers for queue
char **cities = (char* []){"Arad","Bucharest","Craiova","Dobreta",
			"Eforie","Fagaras","Giurgiu","Hirsova",
			"Iasi","","","Lugoj","Mehadia","Nearnt",
			"Oradea","Pitesti","","Rimnieu_Vilcea",
			"Sibiu","Timisoara","Urziceni","Vaslui","",
			"","","Zerind"};
	//Cities are manually entered and stored in a String Array


int main(int argc, char **argv)
{
	if(argc < 4)	//checks if user feeds all necessary input
	{
		printf("\nInsufficient Arguments!!\n\n");
		printf("Correct Input form is:\n");
		printf("\t./[EXECUTABLE] [SEARCH_TYPE] [SOURCE_CITY] [DESTINATION_CITY]\n\n");
	}
	else
	{
		int srcC,destC;
		printf("\nSearch Type: %s\n",argv[1]);
		srcC = getCityId(argv[2]);	//get CityId for Source City
		if (srcC < 0)
		{
			printf("City %s NOT found!\n\n",argv[2]);
			return 0;
		}
		printf("Source City: %s\t",argv[2]);
		destC = getCityId(argv[3]);	//get CityID for Destination City
		if (destC < 0)
		{
			printf("\nCity %s NOT found!\n\n",argv[3]);
			return 0;
		}
		printf("Destination City: %s\n",argv[3]);
		initAdjMat();
		map();
		if (srcC == destC)
		{
			printf("Source City is same as Destination City\n\n");
		}
		// Call appropriate Search Algorithm
                else if ( strcmp("DFS",argv[1]) == 0 )	
                {
                        head = NULL;	
	                DFS(srcC,destC);
                }
                else if( strcmp("BFS",argv[1]) == 0)
                {
                        front = rear = NULL;	
	                BFS(srcC,destC);
                }
                else
                {
                        printf("\nInvalid Search Type\n");
			printf("Search Type can be DFS/BFS\n\n");
                }
	}

	return 0;
}


int getCityId(char *city)	//cityId is usually the index of
{				// array cities where it is saved
	int ret;
	int i;
	ret = -1;
	for(i=0;i<26;i++)
	{
		if( strcmp(city,cities[i]) == 0 )
		{
			ret = i;
			break;
		}
	}
        if(ret == -1)
        {
                printf("City %s NOT found\n",city);
        }
	return ret;
}

void initAdjMat()	//stores the connectivity between cities
{
	int i,j;
	for (i=0; i<26; i++)
	{
		for(j=0; j<26; j++)
		{
			if(i==j)	
			{
				adjMat[i][j]=1;
			}
			else
			{
			adjMat[i][j]=0;
			}
		}
		visited[i] = 0;
	}
}

void printAdjMat()
{
	int i,j;
	for (i=0; i<26; i++)
	{
		for(j=0; j<26; j++)
		{
			printf(" %d",adjMat[i][j]);
		}
		printf("\n");
	}
}

void addRoute(int src, int dest)	//add a new route as declared
{					// 	in map function
	adjMat[src][dest] =  adjMat[dest][src] = 1;
}

void map()	//states which cities are adjacent and stores the same
{		//	in adjacency matrix
	addRoute(getCityId("Oradea"),getCityId("Zerind"));
	addRoute(getCityId("Oradea"),getCityId("Sibiu"));
	addRoute(getCityId("Zerind"),getCityId("Arad"));
	addRoute(getCityId("Sibiu"),getCityId("Fagaras"));
	addRoute(getCityId("Sibiu"),getCityId("Rimnieu_Vilcea"));
	addRoute(getCityId("Arad"),getCityId("Sibiu"));
	addRoute(getCityId("Arad"),getCityId("Timisoara"));
	addRoute(getCityId("Fagaras"),getCityId("Bucharest"));
	addRoute(getCityId("Rimnieu_Vilcea"),getCityId("Pitesti"));
	addRoute(getCityId("Rimnieu_Vilcea"),getCityId("Craiova"));
	addRoute(getCityId("Pitesti"),getCityId("Craiova"));
	addRoute(getCityId("Timisoara"),getCityId("Lugoj"));
	addRoute(getCityId("Lugoj"),getCityId("Mehadia"));
	addRoute(getCityId("Mehadia"),getCityId("Dobreta"));
	addRoute(getCityId("Dobreta"),getCityId("Craiova"));
	addRoute(getCityId("Bucharest"),getCityId("Giurgiu"));
	addRoute(getCityId("Bucharest"),getCityId("Urziceni"));
	addRoute(getCityId("Urziceni"),getCityId("Hirsova"));
	addRoute(getCityId("Hirsova"),getCityId("Eforie"));
	addRoute(getCityId("Urziceni"),getCityId("Vaslui"));
	addRoute(getCityId("Vaslui"),getCityId("Iasi"));
	addRoute(getCityId("Iasi"),getCityId("Nearnt"));
}

void DFS(int src,int dest)
{
        printf("\n-----Depth First Serach---\n\n");
	push(src);
	int success = 0;
	while(!success)
	{
		int beg = pop();	//popping the cityId from stack
		visited[beg] = 1;	//setting visited flag
		int i;
		printf("%s -> ",cities[beg]);
		for(i=0; i<26; i++)
		{
			if(adjMat[beg][i] == 1 && visited[i] != 1)
			{
				if ( i == dest )	//If path found.. Hurray!!
				{
					printf("%s",cities[i]);
					printf("\n\nRoute Found!!\n\n");
					success=1;
				}	
				push(i);	//pushing the neighbors to stack
			}
		}
		if(head == NULL)	//Stack is empty.. Means we were no longer able
		{			//	to traverse over more cities.
			printf("Can NOT reach Destination\n");
			break;
		}
	}
}

void push(int a)
{
	node *temp;
	temp=malloc(sizeof(node));	//Allocate memory space for a node
	temp->data = a;
	if (head != NULL )
	{
		temp->next = head;	//place node on top of stack
	}
	head = temp;
}

int pop()
{
	int ret;
	if (head != NULL)
	{
		ret = head->data;	//get data from top node of stack
		head = head->next;	//move header pointer to next node
		return ret;
	}
	else
	{
		printf("Stack is empty");
		return 0;
	}
	
}

void BFS(int src,int dest)
{
        printf("\n-----Breadth First Serach---\n\n");
	addQ(src);
	int success = 0;
	while(!success)
	{
		int beg = delQ();
		visited[beg] = 1;	//setting visited flag
		int i;
		printf("%s -> ",cities[beg]);
		for(i=0; i<26; i++)
		{
			if(adjMat[beg][i] == 1 && visited[i] != 1)
			{
				if ( i == dest )	//if reached destination.. Hurray!!
				{
					printf("%s",cities[i]);
					printf("\n\nRoute Found!!\n\n");
					success=1;
				}	
				addQ(i);	//adding adjacent cities to queue
			}
		}
		if( emptyQ() )
		{
			printf("No items in Queue\n");
			break;
		}
	}
}

void addQ(int a)
{
	node *temp;
	temp = malloc(sizeof(node));	//allocate memory for new node
	temp->data = a;
	if( emptyQ() )		//add node according to current queue size
	{
		front = rear = temp;
	}
	else
	{
		rear->next = temp;
		rear = temp;
	}
}

int delQ()
{
	int ret;
	ret = front->data;	//get data from from front node
	front = front->next;	
	return ret;
}

int emptyQ()	//check if Queue is empty
{
	if ( front == NULL || rear == NULL )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
