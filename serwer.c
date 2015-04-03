#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include<stdlib.h>
#include <time.h>

# define MAX 64
# define MAX2 10
#define NR 66777

# define indexZ szachownica_ser [MAX]
# define indexO szachownica_ser [MAX+1]
# define indexKZ lista_kli [MAX2]
# define indexKO lista_kli [MAX2+1]
# define indexSZ tab_ser [MAX2]
# define indexSO tab_ser [MAX2+1]

static struct sembuf buf;

struct sserwer{
int nr; //numer identyfikacyjny serewra czyli klucz
int zajetosc; //czy ktos juz gra na serwerze
};

struct sgracz{
int login;
int kolor;
int poziom;
int zajety;
int nr_serwera;
};

typedef struct sserwer komputer;

typedef struct sgracz gracz;

void podnies (int semid , int semnum) {
buf.sem_num = semnum ;
buf.sem_op = 1;
buf.sem_flg = SEM_UNDO;
if(semop(semid,&buf,1)==-1) {
perror (" Podnoszenie semafora ") ;
exit (1) ;
}
}

void opusc(int semid, int semnum) {
buf.sem_num = semnum ;
buf.sem_op = -1;
buf.sem_flg = SEM_UNDO;
if(semop(semid,&buf,1)==-1) {
perror (" Opuszczenie semafora ") ;
exit (1) ;
}
}

main () {

srand(time(0));

int szachownica , semid , semser, semkli, i, lista, serwery;
int *szachownica_ser, *lista_ser;
int zarodek=rand()%89999+10000;
gracz *lista_kli;
komputer *tab_ser;

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


//wpisanie serwera do tablicy serwerów

opusc(semser,0);
opusc(semser,2);


tab_ser[indexSZ.nr].nr=zarodek;
tab_ser[indexSZ.nr].zajetosc=0;

indexSZ.nr = ( indexSZ.nr +1) % MAX2;

podnies(semser,2);
podnies(semser,1);


//tworzenie segmentu pamięci do szachownicy

szachownica = shmget (zarodek , (MAX+2) * sizeof ( int ) , IPC_CREAT |0600) ;
if ( szachownica == -1) {
perror (" Utworzenie segmentu pamieci wspoldzielonej ") ;
exit (1) ;
}

szachownica_ser = ( int *) shmat ( szachownica , NULL , 0) ;
if ( szachownica_ser == NULL ){
perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
exit (1) ;
}


//tworzenie segmentu pamięci do listy klientów

lista = shmget (37373 , (MAX2+2) * sizeof (gracz) , IPC_CREAT |0600) ;
if ( lista == -1) {
perror (" Utworzenie segmentu pamieci wspoldzielonej do listy klientow") ;
exit (1) ;
}

lista_kli = ( gracz *) shmat ( lista , NULL , 0) ;
if ( lista_kli == NULL ){
perror (" Przylaczenie segmentu pamieci wspoldzielonej ") ;
exit (1) ;
}

//tworzenie tablicy semaforów do listy klientów

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


//tworzenie tablicy semaforów

semid = semget (zarodek , 4, IPC_CREAT | IPC_EXCL |0600) ;
if ( semid == -1) {
	semid = semget (zarodek , 4, 0600) ;
	if ( 55767 == -1) {
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

int init=1; //do inicjowania
int pole=0;

while(1) {
opusc ( semid , 0) ;

opusc ( semid , 2) ;

if(init==1){ //inicjowanie

for(pole=0;pole<64;pole++){
if(pole<16||pole>47){szachownica_ser[pole]=1;}
else{szachownica_ser[pole]=0;}
}	

init=0;
}


podnies ( semid , 2) ;
podnies ( semid , 1) ;
}


}



