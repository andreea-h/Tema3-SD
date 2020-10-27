#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define INT_MAX 2000000000

typedef struct ActorList //listele de adiacenta asociate fiecarui nod din graf
{
	int index; //indicele asociat unui actor
	struct ActorList* next;
} ActorList;

typedef struct graph
{
	ActorList** actors;  //vectorul de liste de adiacenta
	int n; //numarul de noduri din graf
	char** nodes; //tablou bidimensional pentru retinerea numelor tuturor actorilo din colectia de filme
} graph;

typedef struct queue //structura de tip coada
{
	int val;
    struct queue* next;
} queue;

typedef struct bTree 
{
	char* name; //numele unui actorului 
	struct bTree* left;
	struct bTree* right;
} bTree;

void freeBTree(bTree* root) //elibereaza memoria alocata pentru nodurile arborelui binar de cautare 'root'
{
	if(root!=NULL)
	{
		freeBTree(root->left);
		freeBTree(root->right);
		free(root->name);
		free(root);
	}
}

void freeActorsList(ActorList** list) //elibereaza memoria alocata pentru nodurile listei de adiacenta 'list'
{
	ActorList* aux=*list; 
   	ActorList* aux1; 
   	while(aux!=NULL)  
   	{ 
       aux1=aux->next; 
       free(aux);
       aux=aux1; 
   	} 
}

void freeGraph(graph* lb) //eliberea memoria alocata pentru nodurile grafului 'lb'
{
	int i;
	for(i=0;i<lb->n;i++) //pentru fiecare nod din graf se eliberea memoria alocata pentru sirul de caractere asociat indexului (numele actorilor)
	{
		free(lb->nodes[i]);
		freeActorsList(&(lb->actors[i])); //se elibereaza memoria alocata pentru listele de adiacenta
	}
	free(lb->nodes);
	free(lb->actors);
	free(lb);
}

void push(queue** q, int val) //adauga un element cu valoarea val in coada q
{
    queue *nou=(queue *)calloc(1,sizeof(queue));
    nou->val=val;
    queue *aux=*q;
    if(*q==NULL) //daca coada este vida
    {
       nou->next=NULL;
       *q=nou;
    }
    else
    {
       while(aux->next!=NULL) //se parcurg elementele din coada pana se ajunge la ultimul
        aux=aux->next;
    aux->next=nou; //adauga un nou element la finalul cozii
    nou->next=NULL;
    }   
}

int pop(queue **q) //elimina un element din coada (ultimul element)
{
    queue *el=*q;
    *q=(*q)->next;
    int rez=el->val;
    free(el);
    return rez;
}

int getPos(graph g, char* name) //returneaza indicele asociat actorului cu numele 'name' din graful g
{
	int i;
    for(i=0;i<g.n;i++)
    	if(strcmp(name,g.nodes[i])==0)
    		return i;
    return -1; //returneza valoarea -1 atunci cand actorul cu numele 'name' nu se gaseste in graf
}

void addEdge(graph *g, int x, int y) 
{
	ActorList* nou1=(ActorList* )malloc(sizeof(ActorList)); //adauga actorul cu indicele y in lista de adiacenta a actorului cu indicele x
	nou1->index=y;
	nou1->next=g->actors[x];
	g->actors[x]=nou1;
	ActorList* nou2=(ActorList* )malloc(sizeof(ActorList)); //adauga actorul cu indicele x in lista de adiacenta a actorului cu indicele y
	nou2->index=x;
	nou2->next=g->actors[y];
	g->actors[y]=nou2;
}

void addVertex(graph* g, char* name) //adauga in graf un nod nou, asociind un index unui nou actor cu numele 'name'
{
	if(g->n==0) //graful nu contine niciun nod, 'name' este primul actor care se adauga in graf
	{
		g->n=g->n+1; //incrementeza numarul de noduri al grafului
		g->nodes=(char** )malloc(sizeof(char *)); //se aloca spatiu pentru numele primului actor din tabloul bidimensional cu numele actorilor
		g->nodes[0]=(char *)malloc((strlen(name)+1)*sizeof(char));
		strcpy(g->nodes[0],name);
	}
	else
	{
		//se cauta actorul in vectorul cu numele actorilor deja adaugati in graf
		int i;
		for(i=0;i<g->n;i++)
			if(strcmp(name,g->nodes[i])==0) //daca actorul a fost deja adaugat in graf
				return;
		g->n=g->n+1;
		g->nodes=(char **)realloc(g->nodes,g->n*sizeof(char *)); //realoca memorie pentru tabloul bidimensional de caractere cu numele actorilor
		g->nodes[g->n-1]=(char *)malloc((strlen(name)+1)*sizeof(char));
		strcpy(g->nodes[g->n-1],name); //introduce un nou actor in graf
	}
}

void readGraph(FILE *fp, graph *g)
{
	int nr_movies; //numarul de filme
	fscanf(fp,"%d\n",&nr_movies);
	char *movie,**actor;
	int i,j,k,nr_actors,index1,index2;
	int nr_prev=0;
	g->actors=(ActorList **)calloc(1,sizeof(ActorList *)); //initial se aloca spatiu pentru o singura lista de adiacenta (initial vida)
	movie=(char *)calloc(60,sizeof(char)); //sir de caractere pentru citirea denumirilor filmelor
	for(i=1;i<=nr_movies;i++)
	{
		fgets(movie,60,fp);
		fscanf(fp,"%d\n",&nr_actors);
		actor=(char**)calloc(nr_actors,sizeof(char*)); //tablou bidimensional cu numele actorilor dintr-un anume film
		for(j=0;j<nr_actors;j++)
			actor[j]=(char *)calloc(60,sizeof(char));
		for(j=0;j<nr_actors;j++)
		{
			fgets(actor[j],60,fp);
			actor[j]=strtok(actor[j],"\n"); //se elimina caraterul '\n'
			addVertex(g,actor[j]); //adauga un nod un graf cu pentru un nou actor cu numele memorat in actor[j]
		}
		g->actors=(ActorList **)realloc(g->actors,g->n*sizeof(ActorList *)); //realoca spatiu pentru listele de adiacenta in graf
		for(int m=nr_prev;m<g->n;m++) 
			g->actors[m]=NULL; //initializeza fiecare lista de adiacenta nou alocata cu NULL
		for(j=0;j<nr_actors;j++) //fiecare actor dintr-un anume film este adaugat in listele de adiacenta ale celorlalti actorilor din acelasi film
		{
			index1=getPos(*g,actor[j]); //index1 este indexul din graf asociat actorul cu numele memorat in 'actor[j]'
			for(k=j+1;k<nr_actors;k++)
			{
				index2=getPos(*g,actor[k]);
				addEdge(g,index1,index2);
			}
		}
		for(j=0;j<nr_actors;j++)
			free(actor[j]);
		free(actor);
		nr_prev=g->n; //nr_prev memoreaza numarul nodurilor din graf inainte de inserarea unor noi actori la citirea datelor unui nou film
	}
	free(movie);
}

void BFS(graph g,int x,int *visited)  //parcurgerea in latime a garfului g, pornind din nodul cu valoarea x
{
	queue* q=NULL; 
    push(&q,x); //adauga nodul de start in coada
    visited[x]=1; //marcheaza nodul de start ca fiind vizitat
    int index=0;
    while(q!=NULL) //atata timp cat coada nu este inca vida
    {
    	index=pop(&q); //extrage un element din coada
    	ActorList* aux=g.actors[index]; //lista de adiacenta a actorului cu idicele 'index', extras din coada
    	while(aux!=NULL)    //se parcurg nodurile adiacente nodului 'index'
    	{
    		if(visited[aux->index]==0)
    		{
    			visited[aux->index]=1;
    			push(&q,aux->index); 
    		}
    		aux=aux->next;
    	}
    }
}

int nrComponents(graph g) //returneaza numarul de componente conexe al grafului g determinat folosind BFS
{
	int k=0;
	int* visited=(int* )calloc(g.n,sizeof(int)); //vectorul 'visited' este initializat cu valori de 0
	for(int i=0;i<g.n;i++) //se aplica algoritmul BFS pentru nodurile nevizitate ale grafului g
	{
		if(visited[i]==0) //nodul de indice i nu a fost visitat
		{
			k++;
			visited[i]=1;
			BFS(g,i,visited);
		}
	}
	free(visited);
	return k;
}

int affinityDegree(graph g,FILE *fp) //retureaza gradul de inrudire al actorilor cu numele actor1 si actor2, cititi din fisier
{
	char* actor1=(char *)calloc(60,sizeof(char));
	fgets(actor1,60,fp);
	actor1=strtok(actor1,"\n"); //este eliminat caraterul '\n'
	char* actor2=(char *)calloc(60,sizeof(char));
	fgets(actor2,60,fp);
	actor2=strtok(actor2,"\n"); //este eliminat caraterul '\n'
	int i=getPos(g,actor1); //i si j sunt indicii asociati actorilor cu numele 'actor1' si 'actor2' din graful g
	int j=getPos(g,actor2);
	//determinam distanta minima de la nodul i la nodul j
	int* distances=(int *)calloc(g.n,sizeof(int));
	int* visited=(int* )calloc(g.n,sizeof(int)); //toate valorile distantelor sunt initial 0
	queue* q=NULL;
	push(&q,i);
	visited[i]=1;
	distances[i]=0; ///'distanta' fata de nodul de start este 0
	int index;
	while(q!=NULL)
	{
		index=pop(&q);
    	ActorList* aux=g.actors[index]; 
    	while(aux!=NULL)
    	{
    		if(visited[aux->index]==0)
    		{
    			visited[aux->index]=1;
    			push(&q,aux->index);
    			distances[aux->index]=distances[index]+1;
    		}
    		aux=aux->next;
    	}
	}
	free(actor1);
	free(actor2);
	int rez=distances[j]; //graful de afinitate este 'distanta' intre nodurile i si j
	free(distances);
	free(visited);
	if(rez==0)
		return 0;
	return rez;
}

bTree* initTree(char* data)
{
	bTree* nou=(bTree*)calloc(1,sizeof(bTree));
	nou->name=(char* )calloc((strlen(data))+1,sizeof(char));
	strcpy(nou->name,data);
	nou->left=NULL;
	nou->right=NULL;
	return nou;
}

void insert(bTree** tree,char* data)
{
	if(*tree==NULL)
		*tree=initTree(data);
	else if(strcmp((*tree)->name,data)>0)
			insert(&(*tree)->left,data);
	else if(strcmp((*tree)->name,data)<0)
		insert(&(*tree)->right,data);
}

void inordine(bTree* points,FILE* fp) //afiseaza valorile din arbore in ordine crescatoare (alfabetic)
{
	if(points==NULL)
		return;
	inordine(points->left,fp);
	fprintf(fp,"%s\n",points->name);
	inordine(points->right,fp);
}

int nrKids(int* parents,int v,int n) //returneaza numarul de copii al nodului v reprezentati prin vectorul 'parents'
{
	int k=0;
	int i;
	for(i=0;i<n;i++)
		if(parents[i]==v)
			k++;
	return k;
}

//memoreaza in arborele binar de cautare 'points' numele actorilor care sunt puncte de articulatie in graful 'g'
void DFSCutVertex(graph* g,int v,int time,int* idx,int* low,int* parents,bTree** points) 
{
	idx[v]=time;
	low[v]=time;
	time=time+1;
	ActorList* aux=g->actors[v]; //aux este lista de noduri adiacente nodului cu valoarea v
	while(aux!=NULL)
	{
		int u=aux->index;
		if(idx[u]==-1) //nodul cu valoarea u nu a fost inca descoperit
		{
			parents[u]=v; //marcheaza nodul u ca fiind copil al nodului v
			DFSCutVertex(g,u,time,idx,low,parents,points); //autoapeleaza functia pentru nodul copil u
			if(low[v]>=low[u])
				low[v]=low[u];
		}
		else
		{
			if(idx[u]<=low[v])
				low[v]=idx[u];
		}
		aux=aux->next;
	}
	if(parents[v]==-1) //v este radacina arborelui care s-ar obtine la parcurgerea in adancime, deci nu are asociat un nod parinte
	{
		if(nrKids(parents,v,g->n)>=2) //daca nodul v are cel putin 2 copii asociati
			insert(points,g->nodes[v]); //nodul v este punct de articulatie
	}
	else
	{
		int i;
		for(i=0;i<g->n;i++)
			if(parents[i]==v) //i este copil al nodului v
			{
				if(low[i]>=idx[v])
				{
					insert(points,g->nodes[v]); //nodul v este punct de articulatie
					break;
				}
			}	
	}
}
 
bTree* cutVertex(graph* g)
{
	bTree* points=NULL; //arbore binar de cautare pentru memorarea actorilor care sunt puncte de arculatie
	int time=0,i;
	int* idx=(int* )calloc(g->n,sizeof(int)); //componentele din 'idx' si 'low' sunt initial 0
	int* low=(int* )calloc(g->n,sizeof(int));
	int* parents=(int* )malloc(g->n*sizeof(int)); //elementele din 'parents' sunt indicii nodurilor 'parinte' rezultate din parcurgerea in adancime a grafului
	for(i=0;i<g->n;i++) 
	{
		idx[i]=-1;
		low[i]=INT_MAX; //o valoare foarte mare
		parents[i]=-1;
	}
	for(i=0;i<g->n;i++)
		if(idx[i]==-1)
			DFSCutVertex(g,i,time,idx,low,parents,&points);
	free(parents);
	free(idx);
	free(low);
	return points;
}

void nrNodes(bTree* points,int *k) // memoreza in k numarul de noduri din arborele 'points'
{
	if(points==NULL)
		return;
	nrNodes(points->left,k);
	(*k)++;
	nrNodes(points->right,k);
}

void convertToTree(graph* g,bTree** tree) //costruieste un arbore binar de cautare alcatuit din nodurile grafului g
{
	int i;
	for(i=0;i<g->n;i++)
		insert(tree,g->nodes[i]);
}

bTree* getSuccesor(bTree* root) //returneaza succesorul in inordine al nodului 'root'
{
	while(root->left!=NULL)
		root=root->left;
	return root;
}

char* minValue(bTree* tree) //returneza nodul cu valoarea minima (primul nod in ordine alfabetica) din arborele 'tree'
{
	if(tree->left!=NULL)
		return minValue(tree->left);
	return tree->name;
	if(tree->right!=NULL)
		return minValue(tree->right);
}

bTree* Remove(bTree* root, char* data) //elimina nodul cu valoarea 'data' din arbore
{
	if(root==NULL || data==NULL)
		return NULL;
	if(strcmp(root->name,data)>0)
		root->left=Remove(root->left,data);
	else if(strcmp(root->name,data)<0)
		root->right=Remove(root->right,data);
	else //cand root indica nodul care trebuie eliminat
	{
		if(root->left!=NULL && root->right!=NULL) //nodul care trebuie eliminat are 2 fii
		{
			bTree* aux=NULL;
			aux=getSuccesor(root->right);
			root->name=(char* )realloc(root->name,strlen(aux->name)+1);
			strcpy(root->name,aux->name); //nodul este inlocuit cu succesorul sau in inordire
			root->right=Remove(root->right,aux->name);
		}
		else //daca nodul are doar un fiu
		{
			bTree* aux=root;
			if(root->left==NULL) //se inlocuieste nodul cu fiul sau drept
			{
				aux=root->right;
				free(root->name);
				free(root);
				root=NULL;
				return aux;
			}
			if(root->right==NULL) //se inlocuieste nodul cu fiul sau stang
			{
				aux=root->left;
				free(root->name);
				free(root);
				root=NULL;
				return aux;
			}
		}
	}
	return root;
}

int findEdge(char* actor1,char* actor2,graph* g) //verifica daca actorul cu numele 'actor2' se gaseste in lista de adiacenta a lui actor1
{
	int poz=getPos(*g,actor1); //pos este indicele asociat actorului cu numele 'actor1' din graful g
	if(poz!=-1)
	{	
		ActorList* aux=g->actors[poz];//lista de adiacenta asociata lui actor1
		while(aux!=NULL)
		{
			if(strcmp(g->nodes[aux->index],actor2)==0)
				return 1;
			aux=aux->next;
		}
	}
	return 0;//nu exista muchie in graf intre cei 2 actori
}

//se adauga in arborele temp toate nodurile din candidates care nu sunt adiacente cu nodul cu numele 'name'
void findXTree(graph* g,char* name,bTree* candidates,bTree** temp)
{
	if(candidates==NULL)
		return;
	if(candidates!=NULL)
	{	
		findXTree(g,name,candidates->left,temp);
		if(candidates->name!=NULL)
			if(findEdge(name,candidates->name,g)==0) //daca nu exista muchie in graf intre nodurile cu valorile 'name' si 'candidates->name'
				insert(temp,candidates->name); //este adaugat nodul cu valoarea 'candidates->name' in arborele 'temp'
		findXTree(g,name,candidates->right,temp);
	}
}

void removeNodes(bTree* candidates,bTree* temp) //elimina din arborele candidates nodurile acestuia care se gasesc si in 'temp'
{
	if(temp==NULL || candidates==NULL)
		return;
	removeNodes(candidates,temp->left);
	candidates=Remove(candidates,temp->name);
	removeNodes(candidates,temp->right);
}

void copyTree(bTree* tree, bTree** copy) //memoreaza in arborele binar 'copy' nodurile arborelui binar 'tree'
{
	if(tree==NULL)
		return;
	copyTree(tree->left,copy);
	insert(copy,tree->name);
	copyTree(tree->right,copy);
}

void printTreeInFile(bTree* tree,FILE* fp)  //afiseaza arborele 'tree' in fisierul identificat prin pointerul 'fp'
{
	if(tree==NULL)
		return;
	printTreeInFile(tree->left,fp);
	fprintf(fp,"%s\n",tree->name);
	printTreeInFile(tree->right,fp);
}

void genClica(graph* g,bTree* clica,bTree* NOT,bTree* candidates,FILE* fp)
{
	if(candidates==NULL && NOT==NULL) //daca candidates si NOT sunt multimi vide, se afiseaza clica maximala
	{
		int nr=0;
		nrNodes(clica,&nr); //nr reprezinta numarul de noduri din arborele 'clica'
		fprintf(fp,"%d\n",nr);
		printTreeInFile(clica,fp);
		fclose(fp);
	}
	if(candidates!=NULL)
	{
		char* name=(char* )malloc(strlen(minValue(candidates))+1);
		strcpy(name,minValue(candidates)); //minValue reprezinta elementul minim din arborele 'candidates'
		candidates=Remove(candidates,name);
		insert(&clica,name);
		bTree* temp1=NULL; //arborele temp1 va contine nodurile din 'candidates' care, in graful g, nu sunt adiacente cu nodul tocmai extras
		findXTree(g,name,candidates,&temp1);
		removeNodes(candidates,temp1); //elimina din 'candidates' nodurile comune cu temp1
		freeBTree(temp1);
		bTree* temp2=NULL; //arborele temp2 va contine nodurile din 'NOT' care, in graful g, nu sunt adiacente cu nodul tocmai extras
		bTree* NOT_next=NULL;
		copyTree(NOT,&NOT_next); //NOT_next contine aceleasi noduri ca si arborele 'NOT'
		findXTree(g,name,NOT,&temp2);
		freeBTree(temp2);
		removeNodes(NOT_next,temp2); //elimina din 'NOT_next' nodurile comune cu temp2
		genClica(g,clica,NOT_next,candidates,fp);
		clica=Remove(clica,name);
		free(name);
	}
}

int main(int argc, char** argv)
{
	graph* Actors;
	Actors=(graph* )calloc(1,sizeof(graph));
	Actors->n=0; //initial graful nu contine niciun nod
	FILE *fp=fopen(argv[2],"r");
	readGraph(fp,Actors);
	//cerinta 1
	if(strstr(argv[1],"c1")!=NULL)
	{
		fclose(fp);
		FILE *fOut=fopen(argv[3],"w");
		fprintf(fOut,"%d\n",nrComponents(*Actors)); //numarul de productii independente
		fclose(fOut);   
		freeGraph(Actors);
	}
	///cerinta 2
	else
		if(strstr(argv[1],"c2")!=NULL)
		{
			FILE *fOut=fopen(argv[3],"w");
			fprintf(fOut,"%d\n",affinityDegree(*Actors,fp)); //gradul de inrudire a doi actori
			fclose(fp);
			fclose(fOut);  
			freeGraph(Actors);
		}
	//cerinta 3
	else
		if(strstr(argv[1],"c3")!=NULL)
		{
			fclose(fp);
		  	FILE *fOut=fopen(argv[3],"w");
			bTree* points=NULL;
			points=cutVertex(Actors); //points este arborele binar de cautare care retine punctele de articulatie din graf
			int nr=0;
			nrNodes(points,&nr); //nr reprezinta numarul de puncte de articulatie din graf
			fprintf(fOut,"%d\n",nr);
			inordine(points,fOut);
			fclose(fOut);
			freeBTree(points); 
			freeGraph(Actors);
		} 
	else //bonus
		if(strstr(argv[1],"b")!=NULL)
		{
			fclose(fp);
			bTree* tree=NULL;
			convertToTree(Actors,&tree); //memoreaza in arborele binar de cautare 'tree' toate nodurile din graful 'Actors'
			bTree* clica=NULL;
			bTree* NOT=NULL;
			FILE *fOut=fopen(argv[3],"w");
			genClica(Actors,clica,NOT,tree,fOut); //se afiseza in fisieul de iesire clica maximala
			freeBTree(NOT);
			freeBTree(clica);  
			freeGraph(Actors);
		}
	return 0;
}