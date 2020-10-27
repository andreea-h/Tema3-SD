#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define INT_MAX 2000000000

typedef struct nod //structura de tip lista pentru retinerea nodurilor adiacente si a costurilor
{
	int time; //valoarea costului - timpul de tranzitie prin portal
	int key; //a doua extremitate a portalului
	struct nod* next;
} nod;

typedef struct maze
{
	nod** rooms; //vector pentru accesarea listelor de adiacenta a fiecarui nod (impreuna cu costurile asociate muchiilor)
	int n; //numarul de camere din labirint
	int m; //numarul de portaluri din labirint
	int* exit; //vector pentru memorarea informatiilor legate de iesirile din labirint
	int** guards; //tablou bidimensional pentru retinerea informatiilor legate de prezenta gardianului intr-o anume camera
} maze;

void freeAdjList(nod* list) //elibereaza memoria alocata pentru lista de adiacenta a unui nod
{
	nod* aux=list;
	nod* aux1=aux;
   	while(aux!=NULL)  
   	{ 
       aux1=aux->next; 
       free(aux);
       aux=aux1; 
   	} 
   	list=NULL;
}

void freeMaze(maze* lb) //elibereaza memoria alocata pentru structura grafului
{
	free(lb->exit);
	int i;
	for(i=0;i<lb->n;i++)
	{
		freeAdjList(lb->rooms[i]); //elibereaza memoria alocata pentru lista de adiacenta a unui nod
		free(lb->guards[i]); 
	}
	free(lb->guards);
	free(lb->rooms);
	free(lb);
}

void initMaze(maze *g,int n,int m) //aloca memorie pentru elementului din structura grafului
{
	g->n=n;
	g->m=m;
	g->rooms=(nod** )calloc(g->n,sizeof(nod* )); //aloca spatiu pentru listele de adiacenta din graf
	g->guards=(int** )calloc(g->n,sizeof(int* ));
	g->exit=(int* )calloc(g->n,sizeof(int));
}

void addPortal(maze* lb,int door1,int door2,int time) //adauga muchie in graf de la nodul door1 la nodul door2
{
	//adauga nodul cu valoarea door2 in lista de adiacenta a nodului door1
	nod* nou=(nod* )malloc(sizeof(nod));
	nou->key=door2;
	nou->time=time;
	nou->next=lb->rooms[door1-1];
	lb->rooms[door1-1]=nou;
}

int readMaze(FILE *fp, maze* lb) //functia returneaza nodul de start al traseului si memoreaza in variabila lb informatiile asociate labirintului
{
	int i,n,m,start,door1,door2,time,guardianPresence;
	char* isExit; //sir de caractere (N\0 sau Y\0) pentru a valida prezenta unei iesiri intr-o anume camera
	isExit=(char* )malloc(2*sizeof(char));
	fscanf(fp,"%d %d %d",&n,&m,&start);
	initMaze(lb,n,m); //alocata memorie pentru listele de adiacenta
	for(i=0;i<m;i++) //adauga muchiile in graf
	{
		fscanf(fp,"%d %d %d",&door1,&door2,&time);
		addPortal(lb,door1,door2,time);
		addPortal(lb,door2,door1,time);
	}
	for(i=0;i<n;i++) //informatii despre fiecare camera din labirint
	{
		fscanf(fp,"%s %d\n",isExit,&guardianPresence);
		if(strstr(isExit,"Y\0")!=NULL) //camera prezinta iesire
			lb->exit[i]=1;
		else
			lb->exit[i]=-1; //camera nu prezinta iesire
		if(guardianPresence==0)
		{
			lb->guards[i]=(int *)malloc(sizeof(int));
			lb->guards[i][0]=-1; //nu este prezent un gardian
		}
		else if(guardianPresence!=0) //se citesc intervalele de timp pentru care este prezent gradianul
		{
			lb->guards[i]=(int *)malloc((guardianPresence+1)*sizeof(int));
			int j,time;
			lb->guards[i][0]=guardianPresence; //numarul momentelor de timp in care este prezent gardinul in camera
			for(j=1;j<guardianPresence+1;j++)
			{
				fscanf(fp,"%d",&time); //sunt citite momentele de timp ale prezentei gardianului
				lb->guards[i][j]=time;
			}
		}
	}
	free(isExit);
	return start; //returneaza valoarea nodului de start
}

//returneaza valoarea nodului 'cu distanta minima' din multimea nodurilor 'nevizitate'
int findMinVertex(int* distances,int* visited,maze* lb)
{
	int i;
	int min=INT_MAX+1,next;
	for(i=0;i<lb->n;i++)
	{
		if(visited[i]==-1 &&  distances[i]<=min)
		{
			min=distances[i]; 
			next=i;
		}
	}
	return next;
}

void findTheWay(maze* lb,int start)  //vectorul way memoreaza nodurile parcurse pana la gasirea iesirii
{
	int i;
	int* visited; //vector pentru marcarea nodurilor pentru care s-au descoperit distantele minime
	visited=(int* )malloc(lb->n*sizeof(int));
	//initial, toate nodurile sunt incluse in multimea de noduri 'nevizitate'
	for(i=0;i<lb->n;i++)
		visited[i]=-1;  //-1 semnifica faptul ca nodul nu a fost 'vizitat'(1- nod visitat)
	int* prev=(int* )malloc(lb->n*sizeof(int));
	for(i=0;i<lb->n;i++) // vectorul prev contine, pentru fiecare nod din traseu, nodul care il preceda 
		prev[i]=-1; 
	prev[start-1]=start-1;
	int *dist=(int* )malloc(lb->n*sizeof(int));
	for(i=0;i<lb->n;i++)
		dist[i]=INT_MAX; //distanta tuturor nodurilor din graf fata de nodul de start (dpdv al traseului de cost minim)
	dist[start-1]=0; //distanta/costul fata de nodul de start este initial 0
	int k=1,min;
	while(k<lb->n)
	{
		min=findMinVertex(dist,visited,lb);
		visited[min]=1;
		if(lb->guards[min][0]!=-1) //se verifica daca in camera este prezent un gardian
		{
			int nr=lb->guards[min][0]; //numarul intervalelor de timp ale prezentei paznicului
			int poz=-1;
			while(poz==-1)
			{
				for(i=1;i<=nr;i++)
					if(lb->guards[min][i]==dist[min])
					{
						poz=i;
						break;
					}
				if(poz!=-1 && lb->exit[min]==-1) //s-a gasit acel moment de timp si camera nu prezinta iesiri
				{	
					dist[min]++;
					poz=-1;
				}
				else 
					poz=0;
			}
		}	
		//se actualizeaza distantele pentru nodurile adiacente nodului min
		nod* neighbour=lb->rooms[min]; //lista nodurilor adiacente nodului min
		while(neighbour!=NULL)
		{
			int cost=dist[min]+neighbour->time;
			int index=neighbour->key-1;
			if(visited[index]==-1 && dist[index]>cost)
			{
				dist[index]=cost;
				prev[index]=min;
			}
			neighbour=neighbour->next;
		}
		k++;
	}
	//se determina timpul minim pentru drumurile catre camere care prezinta iesiri
	int minTime=INT_MAX;
	int finish;
	for(i=0;i<lb->n;i++)
		if(dist[i]<=minTime && lb->exit[i]==1)
		{
			minTime=dist[i];
			finish=i; //finish este indicele iesirii din labirint
		}
	int* way=(int* )malloc(sizeof(int)); //vectorul way va contine camerele traseului in ordine inversa, de la finish la start
	way[0]=finish+1;
	int dim=0;
	while(way[dim]!=start)
	{
		int x=prev[finish]+1;
		way=(int* )realloc(way,(dim+2)*sizeof(int));
		dim++;
		way[dim]=x;
		finish=prev[finish];
	}
	FILE *fOut=fopen("labirint.out","w");
	fprintf(fOut,"%d\n",minTime);
	for(int i=dim;i>=0;i--)
		fprintf(fOut,"%d ",way[i]);
	fprintf(fOut,"\n");
	fclose(fOut);
	free(visited);
	free(prev);
	free(way);
	free(dist);
}

int main()
{
	FILE *fp=fopen("labirint.in","r");
	maze *labyrinth;
	labyrinth=(maze* )malloc(sizeof(maze));
	int start=0; //start reprezinta nodul din care incepe evadarea
	start=readMaze(fp,labyrinth);
	fclose(fp);
	findTheWay(labyrinth,start); //afiseza in fisierul "labirint.out" timpul minim si camerele parcurse
	freeMaze(labyrinth);
	return 0;
}