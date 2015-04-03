#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include<stdlib.h>

# define MAX 64
# define MAX2 10
#define NR 66777

//# define indexZ szachownica_ser [MAX]
//# define indexO szachownica_ser [MAX+1]
# define indexKZ lista_kli [MAX2]
# define indexKO lista_kli [MAX2+1]
# define indexSZ tab_ser [MAX2]
# define indexSO tab_ser [MAX2+1]

static struct sembuf buf;

struct sserwer{
int nr;
int zajetosc;
};

struct sgracz{
int login;
int kolor;
int poziom;
int zajety;
int nr_serwera;
};

struct buf_elem {
long mtype ;
int mvalue ;
};

typedef struct sserwer komputer;

typedef struct sgracz gracz;

typedef struct buf_elem kolejka; 

void podnies ( int semid , int semnum ) {
buf . sem_num = semnum ;
buf . sem_op = 1;
buf . sem_flg = SEM_UNDO;
if ( semop ( semid , & buf , 1) == -1) {
perror ("Podnoszenie semafora") ;
exit (1) ;
}
}

void opusc ( int semid , int semnum ) {
buf . sem_num = semnum ;
buf . sem_op = -1;
buf . sem_flg = SEM_UNDO;
if ( semop ( semid , & buf , 1) == -1) {
perror (" Opuszczenie semafora ") ;
exit (1) ;
}
}

int main () {

int szachownica , semid, semkli, semser, i=0, lista, serwery, miejsce_kli, msgid;
int *szachownica_kli;
gracz *lista_kli;
komputer *tab_ser;
kolejka kol;
int typ;

//tworzenie segmentu pamięci do tablicy serwerów

serwery = shmget (12222 , (MAX2+2) * sizeof ( komputer ) , IPC_CREAT |0600) ;
if ( serwery == -1) {
perror (" Utworzenie segmentu pamieci wspoldzielonej ");
exit (1) ;
}

tab_ser = ( komputer *) shmat ( serwery , NULL , 0) ;
if (tab_ser == NULL ){
perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
exit (1) ;
}

//tworzenie tablicy semaforów do tablicy serwerów

semser = semget (NR, 4, IPC_CREAT | IPC_EXCL |0600) ;
if ( semser == -1) {
	semser = semget (NR , 4, 0600) ;
	if ( semid == -1) {
	perror (" Utworzenie tablicy semaforow do tablicy serwerów") ;
	exit (1) ;
	}
}
else {

	indexSZ.nr = 0;
	indexSZ.zajetosc = 0;
	indexSO.nr = 0;
	indexSO.zajetosc = 0;
	
	if ( semctl ( semser , 0, SETVAL , ( int ) MAX2 ) == -1) {
	perror (" Nadanie wartosci semaforowi 0") ;
	exit (1) ;
	}

	if ( semctl ( semser , 1, SETVAL , ( int ) 0) == -1) {
	perror (" Nadanie wartosci semaforowi 1") ;
	exit (1) ;
	}
	if ( semctl ( semser , 2, SETVAL , ( int ) 1) == -1) {
	perror (" Nadanie wartosci semaforowi 2") ;
	exit (1) ;
	}
	if ( semctl ( semser , 3, SETVAL , ( int ) 1) == -1) {
	perror (" Nadanie wartosci semaforowi 3") ;
	exit (1) ;
	}
}



//tworzenie segmentu pamięci do listy graczy

lista = shmget (37373 , (MAX2+2) * sizeof (gracz) , IPC_CREAT |0600) ;
if ( lista == -1) {
perror (" Utworzenie segmentu pamieci wspoldzielonej do listy graczy") ;
exit (1) ;
}

lista_kli = ( gracz *) shmat ( lista , NULL , 0) ;
if ( lista_kli == NULL ){
perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
exit (1) ;
}

//tworzenie tablicy semaforów do tablicy klientów

semkli = semget (NR+50 , 1, IPC_CREAT | IPC_EXCL |0600) ;
if ( semkli == -1) {
	semkli = semget (NR+50 , 1, 0600) ;
	if ( semkli == -1) {
	perror (" Utworzenie tablicy semaforow do tablicy serwerów") ;
	exit (1) ;
	}

}
else {
	indexKZ.login = 0;
	indexKZ.kolor = 0;
	indexKZ.poziom = 0;
	indexKO.login = 0;
	indexKO.kolor = 0;
	indexKO.poziom = 0;
	
	if ( semctl ( semkli , 0, SETVAL , ( int ) 1 ) == -1) {
	perror (" Nadanie wartosci semaforowi 0") ;
	exit (1) ;
	}
}


//wybór trybu gry - obesrwator/gracz

printf("Wybierz, kim chcesz byc:\n");
printf("1.Gracz\n2.Obserwator");
scanf("%d",&typ);

if(typ==2){//obserwator

opusc(semser,3);

opusc(semkli,0);

for(i=0;i<indexSZ.nr;i++){printf("%d.%d\n",i,tab_ser[i].nr);}
printf("Podaj nr porządkowy serwera do obserwacji:\n");
i=0;
scanf("%d",&i);

lista_kli[miejsce_kli].nr_serwera=tab_ser[i].nr;

podnies(semkli,0);
podnies(semser,3);
//podnies(semser,0);


//tworzenie segmentu pamięci do szachownicy

szachownica = shmget (lista_kli[miejsce_kli].nr_serwera, ( MAX +2) * sizeof (int) , IPC_CREAT |0600) ;
if ( szachownica == -1) {
perror (" Utworzenie segmentu pamieci wspoldzielonej do szachownicy") ;
exit (1) ;
}

szachownica_kli = ( int *) shmat ( szachownica, NULL , SHM_RDONLY) ;
if ( szachownica_kli == NULL ){
perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
exit (1) ;
}


int r;
while(1){

opusc(semser,3);

printf("1.Zobacz aktualny stan szachownicy\n2.Zakoncz obserwacje\n");
scanf("%d",&r);

switch (r){
case 1:
for ( i =0; i <64; i ++){
printf ("%d\t", szachownica_kli[i]);
if(i!=0&&(i+1)%8==0){printf("\n");};
}
break;

case 2:
podnies(semser,3);
exit(0);
break;

default:
break;
}
podnies(semser,3);

}
}

else if(typ==1){//gracz

//pobieranie danych gracza

int login,kolor,poziom;
printf("Podaj swoj nr (1-10):");
scanf("%d",&login);
printf("Podaj kolor pionków:\n0.Czarne\n1.Białe\n");
scanf("%d",&kolor);
printf("Podaj ilość swoich punktów:\n");
scanf("%d",&poziom);

//wpisywanie danych gracza

opusc(semkli,0);

miejsce_kli=indexKZ.login;

lista_kli[miejsce_kli].login=login;
lista_kli[miejsce_kli].kolor=kolor; //1-czarne czyli wybiera, 0-białe czyli jest wybierany
lista_kli[miejsce_kli].poziom=poziom;
lista_kli[miejsce_kli].zajety=0;
lista_kli[miejsce_kli].nr_serwera=0;

indexKZ.login = ( indexKZ.login +1) % MAX ;

podnies(semkli,0);


//wybieranie serwera


if(lista_kli[miejsce_kli].kolor==1){//jeśli czarne

opusc(semser,1);

opusc(semser,3);

lista_kli[miejsce_kli].nr_serwera=tab_ser[indexSO.nr].nr;
indexSO.nr = (indexSO.nr+1)%MAX2;

podnies(semser,3);
podnies(semser,0);
}

//wybieranie partnera

if(lista_kli[miejsce_kli].kolor==1){
opusc(semkli,0);
printf("WYBIERZ NR PRZECIWNIKA:\n");
for(indexKO.login=0;indexKO.login<10;indexKO.login++){
	if(lista_kli[indexKO.login].kolor==0 && lista_kli[indexKO.login].zajety==0 && lista_kli[indexKO.login].login!=0){
	printf("%d.%d\n",indexKO.login,lista_kli[indexKO.login].login);
	}
}

int przeciwnik;

scanf("%d",&przeciwnik);
lista_kli[przeciwnik].nr_serwera=lista_kli[miejsce_kli].nr_serwera;
lista_kli[miejsce_kli].zajety=1;
lista_kli[przeciwnik].zajety=1;
podnies(semkli,0);
}


//tworzenie kolejki do zatrzymywania niesparowanych graczy

msgid = msgget (54543 , IPC_CREAT | IPC_EXCL |0600) ;
if ( msgid == -1) {
	msgid = msgget (54543 , IPC_CREAT |0600) ;
	if ( msgid == -1) {
	perror (" Utworzenie kolejki komunikatów ") ;
	exit (1) ;
	}
}

kol.mtype=lista_kli[miejsce_kli].nr_serwera;
kol.mvalue=1;

//poinformowanie partnera

if(lista_kli[miejsce_kli].kolor==1){//biały wysyła wybranemu czarnemu informację, że mają grać

	if(msgsnd(msgid,&kol,sizeof(kol.mvalue),0)==-1){
		perror("Wysłanie komunikatu");
		exit(1);
	}
}

//wstrzymanie niesparowanych

if(lista_kli[miejsce_kli].kolor==0){//czarny odbiera informację, że został wybrany
	if ( msgrcv ( msgid, &kol, sizeof(kol.mvalue), lista_kli[miejsce_kli].nr_serwera, 0) == -1) {
		perror ("Odebranie komunikatu ") ;
		exit (1) ;
	}
}

//tworzenie segmentu pamięci do szachownicy

szachownica = shmget (lista_kli[miejsce_kli].nr_serwera, (MAX+2) * sizeof (int) , IPC_CREAT |0600) ;
if ( szachownica == -1) {
perror (" Utworzenie segmentu pamieci wspoldzielonej do szachownicy") ;
exit (1) ;
}

szachownica_kli = ( int *) shmat ( szachownica, NULL , 0) ;
if ( szachownica_kli == NULL ){
perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
exit (1) ;
}


//tworzenie tablicy semaforów

semid = semget (lista_kli[miejsce_kli].nr_serwera , 4, IPC_CREAT | IPC_EXCL |0600) ;
if ( semid == -1) {
	semid = semget (lista_kli[miejsce_kli].nr_serwera , 4, 0600) ;
	if ( semid == -1) {
	perror (" Utworzenie tablicy semaforow ") ;
	exit (1) ;
	}

}
else {

	indexKZ.login = 0;
	indexKZ.kolor = 0;
	indexKZ.poziom = 0;
	indexKO.login = 0;
	indexKO.kolor = 0;
	indexKO.poziom = 0;
	indexSZ.nr = 0;
	indexSZ.zajetosc = 0;
	indexSO.nr = 0;
	indexSO.zajetosc = 0;

	if ( semctl ( semid , 0, SETVAL , ( int ) MAX ) == -1) {
	perror (" Nadanie wartosci semaforowi 0") ;
	exit (1) ;
	}

	if ( semctl ( semid , 1, SETVAL , ( int ) 0) == -1) {
	perror (" Nadanie wartosci semaforowi 1") ;
	exit (1) ;
	}
	if ( semctl ( semid , 2, SETVAL , ( int ) 1) == -1) {
	perror (" Nadanie wartosci semaforowi 2") ;
	exit (1) ;
	}
	if ( semctl ( semid , 3, SETVAL , ( int ) 1) == -1) {
	perror (" Nadanie wartosci semaforowi 3") ;
	exit (1) ;
	}

}



int init=1;
int ruchX1=0,ruchY1=0,ruchO=0,ruchX2=0,ruchY2=0;
int x,y;
int Z,C;


while(1) {


opusc ( semid , 1) ;

opusc ( semid , 3) ;


for (i=0; i <64; i ++){
printf ("%d\t", szachownica_kli[i]);
if(i!=0&&(i+1)%8==0){printf("\n");};
}


printf("0.Wykonaj ruch\n1.Wyswietl szachownice\n2.Wyswietl swoje dane\n3.Zakoncz rozgrywke\n");
START:
scanf("%d",&ruchO);
switch(ruchO)
{
case 0:
printf("Ktory pionek przesunac?\n");
printf("Podaj pierwsza wspolrzedna:\n");
scanf("%d",&ruchX1);
printf("Podaj druga wspolrzedna:\n");
scanf("%d",&ruchY1);
printf("Gdzie chcesz postawic pionek?\n");
printf("Podaj pierwsza wspolrzedna:\n");
scanf("%d",&ruchX2);
printf("Podaj druga wspolrzedna:\n");
scanf("%d",&ruchY2);
printf("przyjalem wspolrzedne\n");

Z=(ruchX1-1)*8+ruchY1-1;
C=(ruchX2-1)*8+ruchY2-1;

szachownica_kli[C]=szachownica_kli[Z];
szachownica_kli[Z]=0;
break;

case 1:
for ( i =0; i <64; i ++){
printf ("%d\t", szachownica_kli[i]);
if(i!=0&&(i+1)%8==0){printf("\n");};
}
goto START;
break;

case 2:
printf("LOGIN: %d\n",lista_kli[miejsce_kli].login);
if(lista_kli[miejsce_kli].kolor==1){printf("KOLOR PIONKOW: CZARNE\n");}
else{printf("KOLOR PIONKOW: BIAŁE\n");}
printf("ILOSC PUNKTOW: %d\n",lista_kli[miejsce_kli].poziom);
goto START;
break;

case 3:
podnies ( semid , 3) ;

podnies ( semid , 0) ;

shmdt(szachownica_kli);
shmdt(tab_ser);
shmdt(lista_kli);
msgctl(kol, IPC_RMID, NULL);
exit(0);
break;

}

podnies ( semid , 3) ;

podnies ( semid , 0) ;

}


}
}
