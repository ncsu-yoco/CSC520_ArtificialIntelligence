/*----------------------------------------------

Name : Kapil Mahaveer Somani
Unity ID : kmsomani

CSC 520 - Artificial Intelligence I ( FALL 2014 )
Assignment II
09-29-2014
SearchUSA.c

Implements A*, Greedy and Uniform Cost search algorithms on the 
	USA map to find routes between
	various cities.

Map: SearchUSA.png

-----------------------------------------------*/
/*-----------------------------------------------
NOTE:
Please add -lm command while compiling the code to link math library for building executable.
(for functions sqrt() and cosf() used in the code.)

gcc SearchUSA.c -o op -lm

Thanks!!
-----------------------------------------------*/


//Include necessary libraries

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>


#define maxCities 112	//Total Number of cities 112
#define pathSize 10000
#define connector ","

float PI=3.141593;

/*Node structure which will be used to implement
	queue operations required for traversals
*/
typedef struct node
{
	int data;
	float cost;
	int distTravel;
	char path[pathSize];
	struct node *next;
}node;

typedef struct city
{
	char *name;
	float lat;
	float lon;
}city;

node *front,*rear;	//pointers for queue


int numCities = 0;	
int noExpanded = 0;
char nodesExpanded[pathSize];
char solPath[pathSize];
float solCost;

//Function declarations
void initAdjMat();	//Initializes the Adjacency Matrix
void printAdjMat();	//Prints the Adjacency Matrix
void printCities();
void addRoute(char*,char*,int);	//Add routes between different cities
int getCityId(char *);	//get unique Id for each city
void map();	//Maps the world, uses addCity(char*,float,float) and addRoute(char*,char*,int)
void getNeighbors(int);
void concatStr(char**,char*);
	
float heurVal(int,int);
void aStar(int,int);
void greedy(int,int);
void ucs(int,int);

void addQ(int);		//function of queue
void addPQ(int,float,char*,int);
void repPQ(int,float,char*,int);
int delQ();		//function of queue
node delPQ();
int emptyQ();		//function of queue
void printPQ();
void printSol();

int visited[112];	//stores wether a particular city is visited
int adjMat[112][112];	//Adjacency Matrix

city cities[112];

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
		initAdjMat();
		map();
		//printCities();
		//printAdjMat();
		int srcC,destC;
//		printf("\nSearch Type: %s\n",argv[1]);
		srcC = getCityId(argv[2]);	//get CityId for Source City
		if (srcC < 0)
		{
//			printf("City %s NOT found!\n\n",argv[2]);
			return 0;
		}
//		printf("Source City: %s\t",argv[2]);
		destC = getCityId(argv[3]);	//get CityID for Destination City
		if (destC < 0)
		{
//			printf("\nCity %s NOT found!\n\n",argv[3]);
			return 0;
		}
//		printf("Destination City: %s\n",argv[3]);
		
		if (srcC == destC)
		{
			printf("Source City is same as Destination City - %s\n",argv[2]);
		}
		// Call appropriate Search Algorithm
                else if ( strcmp("astar",argv[1]) == 0 )	
                {
					front = rear = NULL;
					aStar(srcC,destC);
					printSol();
                }
                else if( strcmp("greedy",argv[1]) == 0)
                {
					   greedy(srcC,destC);
					   printSol();
                }
                else if( strcmp("uniform",argv[1]) == 0)
                {
                    front = rear = NULL;	
	                ucs(srcC,destC);
					printSol();
                }
                else
                {
                        printf("\nInvalid Search Type\n");
						printf("Search Type can be astar/greedy/uniform\n\n");
                }
	}

	return 0;
}

int getCityId(char *city)	//cityId is usually the index of
{				// array cities where it is saved
	int ret;
	int i;
	ret = -1;
	for( i = 0 ; i < maxCities ; i++ )
	{
		if( strcmp(city,cities[i].name) == 0 )
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
	for ( i = 0 ; i < maxCities ; i++ )
	{
		for( j = 0 ; j < maxCities ; j++ )
		{
			adjMat[i][j]=0;
		}
		visited[i] = 0;
	}
}

void printAdjMat()
{
	int i,j;
	for ( i = 0 ; i < maxCities ; i++ )
	{
		for( j = 0 ; j < maxCities ; j++ )
		{
			printf(" %d",adjMat[i][j]);
		}
		printf("\n");
	}
}

void printCities()
{
	int i;
	for(i = 0 ; i < maxCities ; i++ )
	{
		printf("City:%s [%f,%f]\n",cities[i].name,cities[i].lat,cities[i].lon);
	}
}

void addRoute(char *src, char *dest, int distance)	//add a new route as declared
{					// 	in map function
//	printf("Distance %f between %s & %s\n",distance,src,dest);
	int srcId = getCityId(src);
	int destId = getCityId(dest);
	if (srcId != -1 && destId != -1)
	{
		adjMat[srcId][destId] =  adjMat[destId][srcId] = distance;
	}
}

void addCity(char *cityName,float latit,float longi)
{
	cities[numCities].name = cityName;
	cities[numCities].lat = latit;
	cities[numCities].lon = longi;
	numCities++;
}

/*
void getNeighbors(int curr)
{
	printf("\t**%s**\n",cities[curr].name);
	for(int i = 0 ; i < maxCities ; i++)
	{
		if (adjMat[curr][i] > 0 )
		{
			printf("\t\t->%s : %d\n",cities[i].name,adjMat[curr][i]);
		}
	}
}
*/

float heurVal(int currCity,int goalCity)
{
	return sqrt(
					pow(69.5*(cities[currCity].lat-cities[goalCity].lat),2) 
						+ 
					pow(69.5*cosf(PI*(cities[currCity].lat+cities[goalCity].lat)/360)*(cities[currCity].lon-cities[goalCity].lon),2)
				);
}

void aStar(int src,int dest)
{
	int curr;
	float cost;
	int flag;
	int distTrav;
	char *currPath;
	
	curr = src;
	cost = 0.0;
	flag = 1;
	currPath = "";
	addPQ(curr,cost,currPath,0);
	while ( !emptyQ() && flag == 1)
	{
		//printPQ();
		node ret;
		ret = delPQ();
		curr = ret.data;
		cost = ret.cost;
		distTrav = ret.distTravel;
//		cost = (int)(ret.cost - heurVal(curr,dest));
		currPath = ret.path;
		visited[curr] = 1;
		int i;
		for( i = 0 ; i < maxCities ; i++ )
		{
			
			if ( adjMat[curr][i] != 0 && !visited[i] )
			{
				if( i == dest )
				{
				//	printf("Path found!!\n");
					strcat(solPath,currPath);
					strcat(solPath,connector);
					strcat(solPath,cities[i].name);
					solCost = distTrav + adjMat[curr][i];
					flag = 0;
					break;
				}
				float currCost = cost + adjMat[curr][i] + heurVal(i,dest);
				addPQ(i, currCost, currPath,distTrav + adjMat[curr][i]);
			}
		}
	}
}

void greedy(int src,int dest)
{
//	printf("finding shortest path between %s & %s using greedy algorithm\n",cities[src].name,cities[dest].name);
//	int ret;
	int curr = src;
	int cost = 0;
	int flag = 1;
	char path[pathSize] = "" ;
	strcat(path,cities[src].name);
//	printf("%s(%d)",cities[src].name,cost);
	visited[src] = 1;
	while(flag == 1)
	{
			//	printf("Checking for %s..\n",cities[curr].name);
		unsigned int min = 65536;
		int closest = curr;
		//getNeighbors(curr);
		int i;
		for(i=0 ; i < maxCities ; i++)
		{
			if ( adjMat[curr][i] != 0 && !visited[i] )
			{
				//printf ("F: %s T: %s Heur %f",cities[curr].name,cities[i].name,heurVal(i,dest));
				//printf("\t%s to %s at %f\n",cities[curr].name,cities[i].name,adjMat[curr][i]);
				if ( i == dest )
				{
					strcat(path,connector);
					strcat(path,cities[i].name);
					cost = cost + adjMat[curr][i];
					strcpy(solPath,path);
					solCost = cost;
					noExpanded++;
					flag = 0;
					break;
				}
				else if ( heurVal(i,dest) < min )
				{
					min = heurVal(i,dest);
					closest = i;
				}
			}
		}
		if ( flag == 1)
		{
			if ( closest != curr )
			{
				cost = cost + adjMat[curr][closest];
				strcat(path,connector);
				strcat(path,cities[closest].name);
				visited[closest] = 1;
				curr = closest;
				noExpanded++;
				strcpy(nodesExpanded,path);
			}
			else
			{
					printf("Cannot find Path\n");
				flag = -1;
			}
		}
		if ( flag != -1 )
		{
		//	printf("Path:{%s} Cost:%d\n",path,cost);	//Prints the output
	 	}
	}
}

void ucs(int src,int dest)
{
	int curr;
	float cost;
	int flag;
	char *currPath;
	int distTrav;
	curr = src;
	cost = 0.0;
	flag = 1;
	currPath = "";
//	printf("UCS\n");
	addPQ(curr,cost,currPath,0);
	while ( !emptyQ() )
	{
		node ret;
		ret = delPQ();
		curr = ret.data;
		cost = ret.cost;
		currPath = ret.path;
		distTrav = ret.distTravel;
		visited[curr] = 1;
		if( curr == dest )
		{
			//printf("Path found!!\n");
			strcat(solPath,currPath);
			solCost = distTrav;
			break;
		}
		int i;
		for( i = 0 ; i < maxCities ; i++ )
		{
			if ( adjMat[curr][i] != 0 && !visited[i] )
			{
				addPQ(i,cost + adjMat[curr][i],currPath,distTrav + adjMat[curr][i]);
			}
			else if ( adjMat[curr][i] != 0 && visited[i] )
			{
				if( !emptyQ() )
				{
					repPQ(i,cost + adjMat[curr][i],currPath,distTrav + adjMat[curr][i]);
				}
			}
		}
	}
}

void printSol()
{
	int count = 0;
	if ( strcmp(solPath,"") != 0 )
	{	
		count++;
		char *s;
		s=solPath;
		s=strstr(s,connector);
		while ( s != NULL )
		{
			count++;
			s++;
			s=strstr(s,connector);
		}
	}
	printf("Expanded Nodes : {%s}\n",nodesExpanded);
	printf("Number of Nodes Expanded : %d\n",noExpanded);
	printf("Solution Path : {%s}\n",solPath);
	printf("Number of Nodes in Solution Path : %d\n",count);
	printf("Solution Cost %.2f\n",solCost);
}

void map()
{
	addCity("albanyGA",     31.58,  84.17);
	addCity("albanyNY",     42.66,  73.78);
	addCity("albuquerque",  35.11, 106.61);
	addCity("atlanta",      33.76,  84.40);
	addCity("augusta",      33.43,  82.02);
	addCity("austin",       30.30,  97.75);
	addCity("bakersfield",  35.36, 119.03);
	addCity("baltimore",    39.31,  76.62);
	addCity("batonRouge",   30.46,  91.14);
	addCity("beaumont",     30.08,  94.13);
	addCity("boise",        43.61, 116.24);
	addCity("boston",       42.32,  71.09);
	addCity("buffalo",      42.90,  78.85);
	addCity("calgary",      51.00, 114.00);
	addCity("charlotte",    35.21,  80.83);
	addCity("chattanooga",  35.05,  85.27);
	addCity("chicago",      41.84,  87.68);
	addCity("cincinnati",   39.14,  84.50);
	addCity("cleveland",    41.48,  81.67);
	addCity("coloradoSprings", 38.86, 104.79);
	addCity("columbus",     39.99,  82.99);
	addCity("dallas",       32.80,  96.79);
	addCity("dayton",       39.76,  84.20);
	addCity("daytonaBeach", 29.21,  81.04);
	addCity("denver",       39.73, 104.97);
	addCity("desMoines",    41.59,  93.62);
	addCity("elPaso",       31.79, 106.42);
	addCity("eugene",       44.06, 123.11);
	addCity("europe",       48.87,  -2.33);
	addCity("ftWorth",      32.74,  97.33);
	addCity("fresno",       36.78, 119.79);
	addCity("grandJunction",39.08, 108.56);
	addCity("greenBay",     44.51,  88.02);
	addCity("greensboro",   36.08,  79.82);
	addCity("houston",      29.76,  95.38);
	addCity("indianapolis", 39.79,  86.15);
	addCity("jacksonville", 30.32,  81.66);
	addCity("japan",        35.68, 220.23);
	addCity("kansas",   39.08,  94.56);
	addCity("keyWest",      24.56,  81.78);
	addCity("lafayette",    30.21,  92.03);
	addCity("lake",     30.19,  82.64);
	addCity("laredo",       27.52,  99.49);
	addCity("lasVegas",     36.19, 115.22);
	addCity("lincoln",      40.81,  96.68);
	addCity("littleRock",   34.74,  92.33);
	addCity("losAngeles",   34.03, 118.17);
	addCity("macon",        32.83,  83.65);
	addCity("medford",      42.33, 122.86);
	addCity("memphis",      35.12,  89.97);
	addCity("mexia",        31.68,  96.48);
	addCity("mexico",       19.40,  99.12);
	addCity("miami",        25.79,  80.22);
	addCity("midland",      43.62,  84.23);
	addCity("milwaukee",    43.05,  87.96);
	addCity("minneapolis",  44.96,  93.27);
	addCity("modesto",      37.66, 120.99);
	addCity("montreal",     45.50,  73.67);
	addCity("nashville",    36.15,  86.76);
	addCity("newHaven",     41.31,  72.92);
	addCity("newOrleans",   29.97,  90.06);
	addCity("newYork",      40.70,  73.92);
	addCity("norfolk",      36.89,  76.26);
	addCity("oakland",      37.80, 122.23);
	addCity("oklahoma", 35.48,  97.53);
	addCity("omaha",        41.26,  96.01);
	addCity("orlando",      28.53,  81.38);
	addCity("ottawa",       45.42,  75.69);
	addCity("pensacola",    30.44,  87.21);
	addCity("philadelphia", 40.72,  76.12);
	addCity("phoenix",      33.53, 112.08);
	addCity("pittsburgh",   40.40,  79.84);
	addCity("pointReyes",   38.07, 122.81);
	addCity("portland",     45.52, 122.64);
	addCity("providence",   41.80,  71.36);
	addCity("provo",        40.24, 111.66);
	addCity("raleigh",      35.82,  78.64);
	addCity("redding",      40.58, 122.37);
	addCity("reno",         39.53, 119.82);
	addCity("richmond",     37.54,  77.46);
	addCity("rochester",    43.17,  77.61);
	addCity("sacramento",   38.56, 121.47);
	addCity("salem",        44.93, 123.03);
	addCity("salinas",      36.68, 121.64);
	addCity("saltLake", 40.75, 111.89);
	addCity("sanAntonio",   29.45,  98.51);
	addCity("sanDiego",     32.78, 117.15);
	addCity("sanFrancisco", 37.76, 122.44);
	addCity("sanJose",      37.30, 121.87);
	addCity("sanLuisObispo",35.27, 120.66);
	addCity("santaFe",      35.67, 105.96);
	addCity("saultSteMarie",46.49,  84.35);
	addCity("savannah",     32.05,  81.10);
	addCity("seattle",      47.63, 122.33);
	addCity("stLouis",      38.63,  90.24);
	addCity("stamford",     41.07,  73.54);
	addCity("stockton",     37.98, 121.30);
	addCity("tallahassee",  30.45,  84.27);
	addCity("tampa",        27.97,  82.46);
	addCity("thunderBay",   48.38,  89.25);
	addCity("toledo",       41.67,  83.58);
	addCity("toronto",      43.65,  79.38);
	addCity("tucson",       32.21, 110.92);
	addCity("tulsa",        36.13,  95.94);
	addCity("uk1",          51.30,0.00);
	addCity("uk2",          51.30,0.00);
	addCity("vancouver",    49.25, 123.10);
	addCity("washington",   38.91,  77.01);
	addCity("westPalmBeach",26.71,  80.05);
	addCity("wichita",      37.69,  97.34);
	addCity("winnipeg",     49.90,  97.13);
	addCity("yuma",         32.69, 114.62);
	addRoute("albanyNY","montreal",226);
	addRoute("albanyNY","boston",166);
	addRoute("albanyNY","rochester",148);
	addRoute("albanyGA","tallahassee",120);
	addRoute("albanyGA","macon",106);
	addRoute("albuquerque","elPaso",267);
	addRoute("albuquerque","santaFe",61);
	addRoute("atlanta","macon",82);
	addRoute("atlanta","chattanooga",117);
	addRoute("augusta","charlotte",161);
	addRoute("augusta","savannah",131);
	addRoute("austin","houston",186);
	addRoute("austin","sanAntonio",79);
	addRoute("bakersfield","losAngeles",112);
	addRoute("bakersfield","fresno",107);
	addRoute("baltimore","philadelphia",102);
	addRoute("baltimore","washington",45);
	addRoute("batonRouge","lafayette",50);
	addRoute("batonRouge","newOrleans",80);
	addRoute("beaumont","houston",69);
	addRoute("beaumont","lafayette",122);
	addRoute("boise","saltLake",349);
	addRoute("boise","portland",428);
	addRoute("boston","providence",51);
	addRoute("buffalo","toronto",105);
	addRoute("buffalo","rochester",64);
	addRoute("buffalo","cleveland",191);
	addRoute("calgary","vancouver",605);
	addRoute("calgary","winnipeg",829);
	addRoute("charlotte","greensboro",91);
	addRoute("chattanooga","nashville",129);
	addRoute("chicago","milwaukee",90);
	addRoute("chicago","midland",279);
	addRoute("cincinnati","indianapolis",110);
	addRoute("cincinnati","dayton",56);
	addRoute("cleveland","pittsburgh",157);
	addRoute("cleveland","columbus",142);
	addRoute("coloradoSprings","denver",70);
	addRoute("coloradoSprings","santaFe",316);
	addRoute("columbus","dayton",72);
	addRoute("dallas","denver",792);
	addRoute("dallas","mexia",83);
	addRoute("daytonaBeach","jacksonville",92);
	addRoute("daytonaBeach","orlando",54);
	addRoute("denver","wichita",523);
	addRoute("denver","grandJunction",246);
	addRoute("desMoines","omaha",135);
	addRoute("desMoines","minneapolis",246);
	addRoute("elPaso","sanAntonio",580);
	addRoute("elPaso","tucson",320);
	addRoute("eugene","salem",63);
	addRoute("eugene","medford",165);
	addRoute("europe","philadelphia",3939);
	addRoute("ftWorth","oklahoma",209);
	addRoute("fresno","modesto",109);
	addRoute("grandJunction","provo",220);
	addRoute("greenBay","minneapolis",304);
	addRoute("greenBay","milwaukee",117);
	addRoute("greensboro","raleigh",74);
	addRoute("houston","mexia",165);
	addRoute("indianapolis","stLouis",246);
	addRoute("jacksonville","savannah",140);
	addRoute("jacksonville","lake",113);
	addRoute("japan","pointReyes",5131);
	addRoute("japan","sanLuisObispo",5451);
	addRoute("kansas","tulsa",249);
	addRoute("kansas","stLouis",256);
	addRoute("kansas","wichita",190);
	addRoute("keyWest","tampa",446);
	addRoute("lake","tampa",169);
	addRoute("lake","tallahassee",104);
	addRoute("laredo","sanAntonio",154);
	addRoute("laredo","mexico",741);
	addRoute("lasVegas","losAngeles",275);
	addRoute("lasVegas","saltLake",486);
	addRoute("lincoln","wichita",277);
	addRoute("lincoln","omaha",58);
	addRoute("littleRock","memphis",137);
	addRoute("littleRock","tulsa",276);
	addRoute("losAngeles","sanDiego",124);
	addRoute("losAngeles","sanLuisObispo",182);
	addRoute("medford","redding",150);
	addRoute("memphis","nashville",210);
	addRoute("miami","westPalmBeach",67);
	addRoute("midland","toledo",82);
	addRoute("minneapolis","winnipeg",463);
	addRoute("modesto","stockton",29);
	addRoute("montreal","ottawa",132);
	addRoute("newHaven","providence",110);
	addRoute("newHaven","stamford",92);
	addRoute("newOrleans","pensacola",268);
	addRoute("newYork","philadelphia",101);
	addRoute("norfolk","richmond",92);
	addRoute("norfolk","raleigh",174);
	addRoute("oakland","sanFrancisco",8);
	addRoute("oakland","sanJose",42);
	addRoute("oklahoma","tulsa",105);
	addRoute("orlando","westPalmBeach",168);
	addRoute("orlando","tampa",84);
	addRoute("ottawa","toronto",269);
	addRoute("pensacola","tallahassee",120);
	addRoute("philadelphia","pittsburgh",319);
	addRoute("philadelphia","newYork",101);
	addRoute("philadelphia","uk1",3548);
	addRoute("philadelphia","uk2",3548);
	addRoute("phoenix","tucson",117);
	addRoute("phoenix","yuma",178);
	addRoute("pointReyes","redding",215);
	addRoute("pointReyes","sacramento",115);
	addRoute("portland","seattle",174);
	addRoute("portland","salem",47);
	addRoute("reno","saltLake",520);
	addRoute("reno","sacramento",133);
	addRoute("richmond","washington",105);
	addRoute("sacramento","sanFrancisco",95);
	addRoute("sacramento","stockton",51);
	addRoute("salinas","sanJose",31);
	addRoute("salinas","sanLuisObispo",137);
	addRoute("sanDiego","yuma",172);
	addRoute("saultSteMarie","thunderBay",442);
	addRoute("saultSteMarie","toronto",436);
	addRoute("seattle","vancouver",115);
	addRoute("thunderBay","winnipeg",440);
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

void addPQ(int a,float calcCost,char *currPath,int currCost)
{
//		printf("***Adding to PQ : %s with %.2f\n ",cities[a].name,calcCost);
	
	node *newNode;
	newNode = malloc(sizeof(node));	//allocate memory for new node
	newNode->data = a;
	newNode->cost = calcCost;
	newNode->distTravel = currCost;
	if( emptyQ() )		//add node according to current queue size
	{
//				printf("Casen 1\n");
		strcat(newNode->path,currPath);
		if ( strcmp(currPath,"") != 0 )
		{
			strcat(newNode->path,connector);
		}
		strcat(newNode->path,cities[a].name);
		front = rear = newNode;
	}
	else if ( front->cost > calcCost )
	{
//				printf("Casen 2\n");
		strcat(newNode->path,currPath);
		strcat(newNode->path,connector);
		strcat(newNode->path,cities[a].name);
		newNode->next = front;
		front = newNode;
	}
	else
	{
//				printf("Casen 3\n");
		strcat(newNode->path,currPath);
		strcat(newNode->path,connector);
		strcat(newNode->path,cities[a].name);
		node *curr, *prev;
		curr = front->next;
		prev = front;
		while(curr != NULL)
		{\
			if( curr->cost > calcCost )
			{
				break;
			}
			curr = curr->next;
			prev = prev->next;
		}
		prev->next = newNode;
		newNode->next = curr;
	}
}

void repPQ(int a,float calcCost,char *currPath,int currCost)
{
	if ( front->data == a )
	{
		if( front->cost > calcCost )
		{
		//	printf("\n\n\n\n\n\n\n\n\n\n R E P   Q U E U E \n\n\n\n\n\n\n\n\n\n\n\n");
			front->cost = calcCost;
			front->distTravel = currCost;
			strcat(front->path,currPath);
			strcat(front->path,connector);
			strcat(front->path,cities[a].name);
		}
		return;
	}
	node *curr;
	node *prev;
	curr = front->next;
	prev = front;
	while( curr != NULL )
	{
		if ( curr->data == a)
		{
			if( curr->cost > calcCost )
			{
			//	printf("\n\n\n\n\n\n\n\n\n\n R E P   Q U E U E \n\n\n\n\n\n\n\n\n\n\n\n");
				prev->next = curr->next;
				addPQ(a,calcCost,currPath,currCost);
			}
			break;
		}
		curr = curr->next;
		prev = prev->next;
	}
}

node delPQ()
{
	node ret;
	ret = *front;
	front = front->next;	
	noExpanded++;
	if( strcmp(nodesExpanded,"") != 0 )
	{
		strcat(nodesExpanded,connector);
	}
	strcat(nodesExpanded,cities[ret.data].name);
//	printf("Deleting from Q\n");
	return ret;
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

void printPQ()
{
	node *temp;
	temp = front;
	printf("*** Q ***\n");
	while( temp != NULL )
	{
		printf("\t => Next:%s Path:{%s} Cost:%.2f\n",cities[temp->data].name,temp->path,temp->cost);
		temp = temp->next;
	}
}