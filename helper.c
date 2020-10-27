typedef struct bTree //arbore binar pentru memorarea punctelor de articulatie din graf
{
	char* name;
	struct bTree* left;
	struct bTree* right;
} bTree;

typedef struct node //structura pentru arborele de parcurgere in adancime
{
	int key;
	struct node* child; //pointer catre primul copil al unui nod
	struct node* sibling; //pointer catre fratele de pe acelasi nivel al copilului unui nod
}node;
typedef struct node *Node;

typedef struct ActorList //listele de adiacenta a fiecarui nod din graf (un nod contine numele unui actor)
{
	char* name;
	struct ActorList* next;
} ActorList;

typedef struct graph
{
	ActorList** actors;  //listele de adiacenta
	int n; //numarul de noduri din graf
	char** nodes; //lista cu numele actorilor din graf
} graph;

typedef struct queue //coada pentru algoritmul de bfs
{
	char* val;
    struct queue* next;
} queue;