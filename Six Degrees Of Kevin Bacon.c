#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node{//her aktör ve film bu node'dan açýlýyor
	int counter;//neighbour sayýsýný tutuyor
	char *name;
	struct node **neighbours;
	struct node *parent;//sadece BFS kýsmýnda kullanýlýyor.Bu node'a gelmeden önceki node u tutup,istenilen path elde ediliyor
};
struct queue{ //queue için structure
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


struct node* addNode(char *name){//Yeni Film ya da aktör ekleme fonksiyonu
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

struct node* searchActor(char *name,int j,struct node **actors){//Aktörlerin olduðu arrayde verilen aktörün pointer'ýný döndüren fonksiyon
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
	enqueue(q,start);//queue'ya ilk node atýlarak baþlanýr
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
				if(visited[j]==temp->neighbours[k]){//eðer O anki node'un i'nci komþusu visited arrayinde varsa queue'ya eklenmez
					flag=1;
				}
			}
			if(flag==0){//flag 0 ise komþuya daha önce gidilmemiþ demektir
				enqueue(q,temp->neighbours[k]);//queue'ya eklenir
				temp->neighbours[k]->parent=temp;//baþlangýç ve hedef arasýndaki path i belirlemek için komþunun parent'ýný o anki node yapýyoruz
			}
		}
		
	}
	return NULL;//Bulunmazsa NULL döndürür
		
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
			printf("Loading...");//Programýn devam ettiðini anlamak için parse edilen her 100000 karakterde bir yükleniyor yazdýrýlýr
			d=0;
			a++;
		}
		else if(a==4){
			printf("\n");
			a=0;
		}
		if(c==' '){//Boþluk karakteri alýnmýyor
			
		}
		else if(c==',' && flag==1){//flag 1'ken virgül geldiyse surname alýmý bitmiþtir
			strcpy(surname,temp);
			temp=(char*)calloc(500,sizeof(char));
			i=0;
		}
		else if((c=='/' && flag==1) || c=='\n'||feof(fp) ){//flag 1'ken "/" geldiyse yeni bir aktöre geçilmiþtir
			strcpy(name,temp);
			temp=(char*)calloc(500,sizeof(char));
			i=0;
			strcpy(temp,name);//name boþluk karakteri ve surname uç uca eklenip 
			strcat(temp," ");
			strcat(temp,surname);
			temp2=searchActor(temp,actorcount,actors);//Ayný isimde biri var mý diye bakýlýr
			if(temp2!=NULL){
				makeNeighbour(temp2,films[j]);//Varsa Film ve aktör komþu yapýlýr
			}
			else{
				actors[actorcount]=addNode(temp);//Yoksa yeni node yaratýlýr
				makeNeighbour(actors[actorcount],films[j]);//komþu yapýlýr
				actorcount++;//Ve aktör sayýsý artýrýlýr
			}
			temp=(char*)calloc(500,sizeof(char));
			if(c=='\n'){// "\n" karakteri yeni filme geçeceðini iþaret eder
				flag=0;
				j++;
				}
			
		}
		else if(c=='/' && flag==0){//flag 0'ken "/" geldiyse film ismi alýmý bitmiþ demektir
			strcpy(film,temp);
			temp=(char*)calloc(500,sizeof(char));
			i=0;
			flag=1;
			films[j]=addNode(film);
		}
		else{
			temp[i]=c;//Hiçbir koþulu saðlamýyorsa alýnan harf tempin sonuna eklenir ve sonraki adýmlarda hangi koþulu saðlýyorsa ona göre name surname yada film olur
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
		printf("%s-%s:%s\n",temp->name,temp->parent->parent->name,temp->parent->name);  //sýrayla temp null olana kadar parentlarý bastýrýlýr
		i++;
		temp=temp->parent->parent;
	}
	printf("%s\n",temp->name);
	printf("Kevin Bacon number is:%d\n\n\n",i);//film-aktör-film-aktör sýrasýyla gidildiði için toplam traverse sayýsýnýn yarýsý KBN sayýsýný verir
}

int ActorNumberInt(struct node *temp,struct node *kevin){//Bir yukarýdaki fonksiyonun KBN döndüren hali
	struct node *checker;//Bu fonksiyonun üsttekinden bir diðer farký path bastýrmamasýdýr
	checker=BFS(kevin,temp);
	int i=0;
	if(checker==NULL){
		return 14;//Aktör alt aðda bulunmuyorsa KBN sonsuz olduðunu belirtmek için 14 döndürdüm
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
		if(i==14){//sonsuz olanlar 14 döndürdüðü için array[14] sonsuzlarý veriyor
			printf("\n%d people have infinite Kevin Bacon Number",array[i]);
		}
		else if(array[i]!=0){//Eðer array[i] 0 deðilse Herhangi bir aktör i KBN sayýsýna sahiptir
			printf("\n%d people have %d Kevin Bacon Number",array[i],i);
		}
	}
}




int main(){
	printf("Select the file to read from:");
	char filename[100];
	scanf("%s",filename);
	struct node **films=(struct node**)malloc(sizeof(struct node)*14400);//Filmleri tutan array
	struct node **actors=(struct node**)malloc(sizeof(struct node)*100000);//aktörleri tutan array
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
		fflush(stdin);//Alýmlar yapýlýrken Ýnput iþlem sonunda enter'a basýlýnca buffer'da \n karakteri kalýyor ve bu durum önce switch:default kýsmýnýn çalýþmasýna neden oluyor
		state=getchar();//Bu durumun olmamasý için console buffer'ýný temizledim
		switch(state){
			case '1':
				printf("Enter the actor to find his/her Kevin Bacon Number:\n(Name (space) Surname)\n ");
				scanf("%s %s",name,surname);
				strcat(name," ");
				strcat(name,surname);
				temp=searchActor(name,actorcount,actors);//Aktörün ismiyle eþleþen biri var mý diye bakýlýr
				if(temp==NULL){
					printf("This Actor doesn't exist in this file");//Bulunamazsa bu hatayý verir
					break;
				}
				kevin=searchActor("Kevin Bacon",actorcount,actors);
				ActorNumber(temp,kevin);//Kevin Bacon ve girilen aktör arasýndaki numara ve path bulunur
				break;
			case '2':
				printf("\nThis may take a while\n");
				FindHowMany(actors,actorcount);//aktörlerin bulunduðu actors arrayi ve actorcount fonksiyona yollanýp her KBN için kaç aktör düþtüðü ekrana basýlýr
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
