#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node{//her akt�r ve film bu node'dan a��l�yor
	int counter;//neighbour say�s�n� tutuyor
	char *name;
	struct node **neighbours;
	struct node *parent;//sadece BFS k�sm�nda kullan�l�yor.Bu node'a gelmeden �nceki node u tutup,istenilen path elde ediliyor
};
struct queue{ //queue i�in structure
    int front, rear, size; 
    int capacity; 
    struct node **array; 
}; 
struct queue* createqueue(int capacity) { 
    struct queue* q = (struct queue*) malloc(sizeof(struct queue)); 
    q->capacity = capacity; 
    q->front = q->size = 0;  
    q->rear = capacity - 1; 
    q->array = (struct node**) malloc(q->capacity * sizeof(struct node)); 
    return q; 
} 
int isFull(struct queue* q){
  return (q->size == q->capacity);  
  }  
int isEmpty(struct queue* q){
  return (q->size == 0); 
  } 
  void enqueue(struct queue* q, struct node *item){ 
    if (isFull(q)) 
        return; 
    q->rear = (q->rear + 1)%q->capacity; 
    q->array[q->rear] = item; 
    q->size = q->size + 1; 
}
struct node* dequeue(struct queue* q) 
{ 
    if (isEmpty(q)) 
        return NULL; 
    struct node *item = q->array[q->front]; 
    q->front = (q->front + 1)%q->capacity; 
    q->size = q->size - 1; 
    return item; 
} 


struct node* addNode(char *name){//Yeni Film ya da akt�r ekleme fonksiyonu
	struct node *newnode;
	newnode=(struct node*)malloc(sizeof(struct node));
	newnode->neighbours=(struct node**)malloc(sizeof(struct node));
	newnode->counter=0;
	newnode->name=(char*)malloc(sizeof(char)*100);
	strcpy(newnode->name,name);
	return newnode;
}

void makeNeighbour(struct node *p1,struct node *p2){//Var olan iki node'u neighbor yapma fonksiyonu
	p1->counter++;
	p2->counter++;
	p1->neighbours=(struct node**)realloc(p1->neighbours,p1->counter*sizeof(struct node));
	p2->neighbours=(struct node**)realloc(p2->neighbours,p2->counter*sizeof(struct node));
	p1->neighbours[p1->counter-1]=p2;
	p2->neighbours[p2->counter-1]=p1;
}

struct node* searchActor(char *name,int j,struct node **actors){//Akt�rlerin oldu�u arrayde verilen akt�r�n pointer'�n� d�nd�ren fonksiyon
	int i;
	int k;
	struct node *temp;
	for(i=0;i<j;i++){
		if(strcmp(actors[i]->name,name)==0){
			temp=actors[i];
			return temp;
		}
	}
	return NULL;
}
struct node* BFS(struct node *start,struct node *target){//Breadth first search fonksiyonu
	struct node **visited=(struct node**)malloc(sizeof(struct node)*150000);
	int i=0;
	struct queue *q=createqueue(150000);
	struct node *temp;
	int j=0;
	int k=0;
	int flag=0;
	enqueue(q,start);//queue'ya ilk node at�larak ba�lan�r
	while(!isEmpty(q)){
		temp=dequeue(q);
		if(temp==target){
			return temp;
		}
		visited[i]=temp;//O anki node visited arrayine eklenir
		i++;	
		for(k=0;k<temp->counter;k++){
			flag=0;
			for(j=0;j<i;j++){
				if(visited[j]==temp->neighbours[k]){//e�er O anki node'un i'nci kom�usu visited arrayinde varsa queue'ya eklenmez
					flag=1;
				}
			}
			if(flag==0){//flag 0 ise kom�uya daha �nce gidilmemi� demektir
				enqueue(q,temp->neighbours[k]);//queue'ya eklenir
				temp->neighbours[k]->parent=temp;//ba�lang�� ve hedef aras�ndaki path i belirlemek i�in kom�unun parent'�n� o anki node yap�yoruz
			}
		}
		
	}
	return NULL;//Bulunmazsa NULL d�nd�r�r
		
}
	
int read_and_process(char* filename,struct node **films,struct node **actors){
	FILE *fp;
	fp=fopen(filename,"r");
	char *temp;
	char *surname;
	char *name;
	char *film;
	temp=(char*)malloc(sizeof(char)*500);
	surname=(char*)malloc(sizeof(char)*500);
	name=(char*)malloc(sizeof(char)*500);
	film=(char*)malloc(sizeof(char)*500);
	struct node *temp2;
	int i=0;
	int j=0;
	int cur=0;
	int a=0;
	int actorcount=0;
	int d=0;
	char c;
	int flag=0;
	while(!feof(fp)){
	c=getc(fp);
		d++;
		if(d==100000){
			printf("Loading...");//Program�n devam etti�ini anlamak i�in parse edilen her 100000 karakterde bir y�kleniyor yazd�r�l�r
			d=0;
			a++;
		}
		else if(a==4){
			printf("\n");
			a=0;
		}
		if(c==' '){//Bo�luk karakteri al�nm�yor
			
		}
		else if(c==',' && flag==1){//flag 1'ken virg�l geldiyse surname al�m� bitmi�tir
			strcpy(surname,temp);
			temp=(char*)calloc(500,sizeof(char));
			i=0;
		}
		else if((c=='/' && flag==1) || c=='\n'||feof(fp) ){//flag 1'ken "/" geldiyse yeni bir akt�re ge�ilmi�tir
			strcpy(name,temp);
			temp=(char*)calloc(500,sizeof(char));
			i=0;
			strcpy(temp,name);//name bo�luk karakteri ve surname u� uca eklenip 
			strcat(temp," ");
			strcat(temp,surname);
			temp2=searchActor(temp,actorcount,actors);//Ayn� isimde biri var m� diye bak�l�r
			if(temp2!=NULL){
				makeNeighbour(temp2,films[j]);//Varsa Film ve akt�r kom�u yap�l�r
			}
			else{
				actors[actorcount]=addNode(temp);//Yoksa yeni node yarat�l�r
				makeNeighbour(actors[actorcount],films[j]);//kom�u yap�l�r
				actorcount++;//Ve akt�r say�s� art�r�l�r
			}
			temp=(char*)calloc(500,sizeof(char));
			if(c=='\n'){// "\n" karakteri yeni filme ge�ece�ini i�aret eder
				flag=0;
				j++;
				}
			
		}
		else if(c=='/' && flag==0){//flag 0'ken "/" geldiyse film ismi al�m� bitmi� demektir
			strcpy(film,temp);
			temp=(char*)calloc(500,sizeof(char));
			i=0;
			flag=1;
			films[j]=addNode(film);
		}
		else{
			temp[i]=c;//Hi�bir ko�ulu sa�lam�yorsa al�nan harf tempin sonuna eklenir ve sonraki ad�mlarda hangi ko�ulu sa�l�yorsa ona g�re name surname yada film olur
			i++;	
		}
	}
	fclose(fp);
	return actorcount;
}
	
void ActorNumber(struct node *temp,struct node *kevin){//Return'u olmayan Bulma ve ekrana yazma fonksiyonudur
	FILE *fp;
	fp=fopen("Parsed.txt","r");
	struct node *checker;
	checker=BFS(kevin,temp);
	int i=0;
	if(checker==NULL){
		printf("\nActor's KBN is infinite");
		return;
	}
	fclose(fp);
	while(temp!=NULL && temp!=kevin){
		printf("%s-%s:%s\n",temp->name,temp->parent->parent->name,temp->parent->name);  //s�rayla temp null olana kadar parentlar� bast�r�l�r
		i++;
		temp=temp->parent->parent;
	}
	printf("%s\n",temp->name);
	printf("Kevin Bacon number is:%d\n\n\n",i);//film-akt�r-film-akt�r s�ras�yla gidildi�i i�in toplam traverse say�s�n�n yar�s� KBN say�s�n� verir
}

int ActorNumberInt(struct node *temp,struct node *kevin){//Bir yukar�daki fonksiyonun KBN d�nd�ren hali
	struct node *checker;//Bu fonksiyonun �sttekinden bir di�er fark� path bast�rmamas�d�r
	checker=BFS(kevin,temp);
	int i=0;
	if(checker==NULL){
		return 14;//Akt�r alt a�da bulunmuyorsa KBN sonsuz oldu�unu belirtmek i�in 14 d�nd�rd�m
	}
	
	while(temp!=NULL && temp!=kevin){
		temp=temp->parent;
		i++;
	}
	return (i/2);
}

void FindHowMany(struct node **actors,int actorcount){
	int i,j,k;
	int array[15];
	for(i=0;i<15;i++){
		array[i]=0;//array 0 olarak initialize edildi
	}
	struct node *kev;
	kev=searchActor("Kevin Bacon",actorcount,actors);
	for(i=0;i<actorcount;i++){
		array[ActorNumberInt(actors[i],kev)]++;
	}
	for(i=0;i<15;i++){
		if(i==14){//sonsuz olanlar 14 d�nd�rd��� i�in array[14] sonsuzlar� veriyor
			printf("\n%d people have infinite Kevin Bacon Number",array[i]);
		}
		else if(array[i]!=0){//E�er array[i] 0 de�ilse Herhangi bir akt�r i KBN say�s�na sahiptir
			printf("\n%d people have %d Kevin Bacon Number",array[i],i);
		}
	}
}




int main(){
	printf("Select the file to read from:");
	char filename[100];
	scanf("%s",filename);
	struct node **films=(struct node**)malloc(sizeof(struct node)*14400);//Filmleri tutan array
	struct node **actors=(struct node**)malloc(sizeof(struct node)*100000);//akt�rleri tutan array
	int actorcount;
	printf("\nThis may take a while(about 10 mins)\n");
	actorcount=read_and_process(filename,films,actors);
	int i=0;
	int j=0;
	struct node *temp;
	struct node *kevin;
	char *name=(char*)malloc(sizeof(char)*15);
	char *surname=(char*)malloc(sizeof(char)*15);
	int flag=1;
	char state;
	do{
		printf("\nTo find an actor's Kevin Bacon number, press '1': \n");
		printf("\nTo find how many actors have which Kevin Bacon number, press '2': \n");
		printf("\nPress '.' to exit\n");
		fflush(stdin);//Al�mlar yap�l�rken �nput i�lem sonunda enter'a bas�l�nca buffer'da \n karakteri kal�yor ve bu durum �nce switch:default k�sm�n�n �al��mas�na neden oluyor
		state=getchar();//Bu durumun olmamas� i�in console buffer'�n� temizledim
		switch(state){
			case '1':
				printf("Enter the actor to find his/her Kevin Bacon Number:\n(Name (space) Surname)\n ");
				scanf("%s %s",name,surname);
				strcat(name," ");
				strcat(name,surname);
				temp=searchActor(name,actorcount,actors);//Akt�r�n ismiyle e�le�en biri var m� diye bak�l�r
				if(temp==NULL){
					printf("This Actor doesn't exist in this file");//Bulunamazsa bu hatay� verir
					break;
				}
				kevin=searchActor("Kevin Bacon",actorcount,actors);
				ActorNumber(temp,kevin);//Kevin Bacon ve girilen akt�r aras�ndaki numara ve path bulunur
				break;
			case '2':
				printf("\nThis may take a while\n");
				FindHowMany(actors,actorcount);//akt�rlerin bulundu�u actors arrayi ve actorcount fonksiyona yollan�p her KBN i�in ka� akt�r d��t��� ekrana bas�l�r
				break;
			case '.':
				flag=0;
				break;
			default:
				printf("\n Invalid input!\n\n");
		}
	}while(flag);
		return 0;
}
