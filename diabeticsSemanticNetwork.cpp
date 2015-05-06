/*----------------------------------------------

Name : Kapil Mahaveer Somani
Unity ID : kmsomani

CSC 520 - Artificial Intelligence I ( FALL 2014 )
Assignment III
10-22-2014
prolog.c

Implements prolog like mechanism for given diabetic network.

SemanticNetwork: diabeticsSemanticNetwork.png

Prolog Rules:
	edge(david, isa, diabetics).
	edge(diabetics, shouldAvoid, sugar).
	edge(candy, contains, sugar).
	edge(snickers, ako, candy).

	value(Node, Slot, Value):- edge(Node, Slot, Value).
	value(Node, Slot, Value):- edge(Node, isa, Node1),
	    			value(Node1, Slot, Value).
	value(Node, Slot, Value):- edge(Node, ako, Node1),
	    			value(Node1, Slot, Value).
	value(Node, shouldAvoid, Node1):- value(Node1, contains, Stuff),
	  	  			value(Node, shouldAvoid, Stuff).

-----------------------------------------------*/

#include <iostream>
#include <string.h>

using namespace std;

string edges[10][3];
int noEdges = 0;

int nodeVarCnt;
int relVarCnt;
int valVarCnt;
string nodeVar[4];
string relVar[4];
string valVar[3];

int solCount;
string solutions[20][3];

void addEdge(string,string,string);
void printEdges();
string edge(string,string,string);
string eval(string,string,string);
string findNode(string,string);
void synth(string);
void findSolutions();
void printSol(string,string,string);
	
int main()
{
	/*
		Adding all the information required to query the diabetics seantic network
	*/
	addEdge("David","isA","diabetics");
	addEdge("diabetics","shouldAvoid","sugar");
	addEdge("candy","contains","sugar");
	addEdge("snickers","ako","candy");
	printEdges();
	/*
	string ret;
	ret=eval("David","isA","diabetics");
	cout<<ret<<endl;
	cout<<"St2"<<endl;
	ret=eval("David","shouldAvoid","sugar");
	cout<<"St3"<<endl;
	ret=eval("snickers","contains","sugar");
	cout<<ret<<endl;
	cout<<"St4"<<endl;
	ret=eval("diabetics","shouldAvoid","snicker");
	cout<<ret<<endl;
	*/
	/*
		A while loop which will keep of accepting queries from user and return values.
		typing 'exit' will quit the prolog console.
	*/
	while(1)
	{
		string query;
		cout<<"?-";
		getline(cin,query);
		if(query != "")
		{
				synth(query);
		}
	}
	return 1;
}

/*
	Synthesize the query inserted by user and initialize necessary variables for proper working.
*/
void synth(string qry)
{
	nodeVarCnt = 0;
	relVarCnt = 0;
	valVarCnt = 0;
	solCount = 0;
	if(qry == "exit")
	{
		exit(0);
	}
	if(qry.back() != '.')
	{
		cout<<"Not a valid syntax"<<endl;
		return;
	}
	char *query = &qry[0];
	char *tokens;
	int i=0;
	string params[4];
	tokens = strtok(query,"(),.");
	while(tokens!=NULL)
	{
		params[i]=string(tokens);
//		cout<<tokens<<endl;
		i++;
		if(i == 4)
		{
			break;
		}
		tokens = strtok(NULL,"(),.");
	}
	if(params[0] == "" || params[1] == "" || params[2] == "" || params[3] == "")
	{
		cout<<"Not a valid syntax"<<endl;
		return;
	}
	if(params[0] != "value")
	{
		cout<<"Not a valid query"<<endl;
		return;
	}
//	cout<<params[0]<<","<<params[1]<<","<<params[2]<<","<<params[3]<<","<<params[4]<<endl;
	int flag=0;
	if(params[1]=="David" || params[1]=="diabetics" || params[1]=="sugar" || params[1]=="candy" || params[1]=="snickers")
	{
		nodeVar[0]=params[1];
		nodeVarCnt=1;
	}
	else
	{
		nodeVar[0]="David";
		nodeVar[1]="diabetics";
		nodeVar[2]="candy";
		nodeVar[3]="snickers";
		nodeVarCnt=4;
	}
	if(params[2]=="isA" || params[2]=="shouldAvoid" || params[2]=="contains" || params[2]=="ako")
	{
		relVar[0]=params[2];
		relVarCnt=1;
	}
	else
	{
		relVar[0]="isA";
		relVar[1]="shouldAvoid";
		relVar[2]="contains";
		relVar[3]="ako";
		relVarCnt=4;
	}
	if(params[3]=="David" || params[3]=="diabetics" || params[3]=="sugar" || params[3]=="candy" || params[3]=="snickers")
	{
		valVar[0]=params[3];
		valVarCnt=1;
	}
	else
	{
		valVar[0]="diabetics";
		valVar[1]="sugar";
		valVar[2]="candy";
		valVarCnt=3;
	}
	if (nodeVarCnt==1 && relVarCnt==1 && valVarCnt==1)
	{
		cout<<"-"<<eval(params[1],params[2],params[3])<<endl;
	}
	else
	{
		/*
		cout<<"Nodes:"<<nodeVarCnt<<endl;
		cout<<"Rels:"<<relVarCnt<<endl;
		cout<<"Values:"<<valVarCnt<<endl;
		*/
	//	cout<<"findSolution";
		findSolutions();
	//	cout<<"printSolution";
		printSol(params[1],params[2],params[3]);
		cout<<endl;
	}
}

/*
	This function adds new information to the system
*/
void addEdge(string node1, string rel, string node2)
{
	edges[noEdges][0] = node1;
	edges[noEdges][1] = rel;
	edges[noEdges][2] = node2;
	noEdges++;
}

/*
	Prints all the edges of the semantic network
*/
void printEdges()
{
	int i;
	for(i=0;i<noEdges;i++)
	{
		cout<<"edge("<<edges[i][0]<<","<<edges[i][1]<<","<<edges[i][2]<<")"<<endl;
//		printf("edge(%s,%s,%s)\n",edges[i][0],edges[i][1],edges[i][2]);
	}
}


/*
	Function returns true if there exists a edge with values passed in parameters,
	returns false otherwise.
*/
string edge(string node1, string rel,string node2)
{
	int i;
	for(i=0;i<noEdges;i++)
	{
		if(edges[i][0]==node1 && edges[i][1]==rel && edges[i][2]==node2)
		{
			return "true";
		}
	}
	return "false";
}

/*
	A function returns true if the list of parameters satisfies on the mentioned rule(there might be a recursive
			call to this function),
	returns false otherwise.
	eval function checks for followinf rules:
	(i) value(Node, Slot, Value):- edge(Node, Slot, Value).
	(ii) value(Node, Slot, Value):- edge(Node, isa, Node1), value(Node1, Slot, Value).
	(iii) value(Node, Slot, Value):- edge(Node, ako, Node1), value(Node1, Slot, Value).
	(iv) value(Node, shouldAvoid, Node1):- value(Node1, contains, Stuff), value(Node, shouldAvoid, Stuff).
*/
string eval(string node,string slot,string value)
{
//		cout<<"\tEval"<<endl;
	string ret;
	string node1;
	if (edge(node,slot,value) == "true")
	{
//				cout<<"Rule1"<<endl;
		return "true";
	}
	node1 = findNode(node,"isA");
//		cout<<"NodeIsA:"<<node1<<endl;
	if(node1 != "false" )
	{
//				cout<<"Rule2"<<endl;
		if (eval(node1,slot,value) == "true")
		{
			return "true";
		}
	}
	node1 = findNode(node,"ako");
//		cout<<"NodeAko:"<<node1<<endl;
	if(node1 != "false")
	{
//				cout<<"Rule3"<<endl;
		if (eval(node1,slot,value) == "true")
		{
			return "true";
		}
	}
	if(slot == "shouldAvoid")
	{
//				cout<<"Rule4"<<endl;
		if (eval(value,"contains","sugar")=="true" && eval(node,"shouldAvoid","sugar")=="true")
		{
			return "true";
		}
	}
	return "false";
}

string findNode(string node,string rel)
{
	int i;
	for(i=0;i<noEdges;i++)
	{
		if(edges[i][0]==node && edges[i][1]==rel)
		{
			return edges[i][2];
		}
	}
	return "false";
}

/*
	Finds solution if one of parameters is a varibale(i.e. not contained in the semantic netwok)
*/
void findSolutions()
{
	int i,j,k;
	for(i=0;i<nodeVarCnt;i++)
	{
		for(j=0;j<relVarCnt;j++)
		{
			for(k=0;k<valVarCnt;k++)
			{
		//		cout<<"("<<nodeVar[i]<<","<<relVar[j]<<","<<valVar[k]<<")"<<endl;
				if(eval(nodeVar[i],relVar[j],valVar[k]) == "true")
				{
		//			cout<<"Satisfies("<<nodeVar[i]<<","<<relVar[j]<<","<<valVar[k]<<")"<<endl;
					solutions[solCount][0]=nodeVar[i];
					solutions[solCount][1]=relVar[j];
					solutions[solCount][2]=valVar[k];
					solCount++;
				}
			}
		}
	}
}

/*
	Prints all the solutions fetched from the findSolution()
*/
void printSol(string node,string rel,string val)
{
	int i = 0;
	while(i<solCount)
	{
		if(nodeVarCnt > 1)
		{
			cout<<node<<"-"<<solutions[i][0];
			getchar();
		}
		if(relVarCnt > 1)
		{
			cout<<rel<<"-"<<solutions[i][1];
			getchar();
		}
		if(valVarCnt > 1)
		{
			cout<<val<<"-"<<solutions[i][2];
			getchar();
		}
		i++;
	}
}