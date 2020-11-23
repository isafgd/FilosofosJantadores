#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>


#define PENSANDO 0 //O filósofo está pensando
#define FAMINTO 1 //O filósofo está tentando pegar os garfos
#define COMENDO 2 //O filósofo está comendo

#define TRUE 1
#define MAX 20

void* filosofo (void* arg);
void pensa (int i);
void come (int i);
void pega_garfos (int i);
void devolve_garfos (int i);
void tentativa (int i);

int n_filosofos; //Número de filósofos
int n_porcoes; //Número de porções
int estado[MAX]; //Vetor para o estado de cada filósofo
sem_t mutex; //Semáforo para regiões críticas
sem_t sem[MAX]; //Semáforos para os filósofos
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

//Função principal: Salva o número de filósofos e a quantidade de macarrão
int main (){
	int i;

	printf ("Número de filósofos sentados à mesa: ");
	scanf ("%d", &n_filosofos);
	printf ("Quantidade de porções de macarrão disponível: ");
	scanf ("%d", &n_porcoes);

	sem_init(&mutex, TRUE, 1); //Inícia o semáforo para regiões críticas

	for (i = 0; i < n_filosofos; i++){
		sem_init(&sem[i], TRUE, 1); //Inicia o semáforo de cada filósofo
	}

	pthread_t thread_filosofo[n_filosofos]; //Declara a thread dos filósofos

	for (i = 0; i < n_filosofos; i++){
		pthread_create (&thread_filosofo[i], NULL, (void *) filosofo, (void *) &i); //Cria cada thread de filósofo 
	}

	for(i = 0; i < n_filosofos; i++) {
     pthread_join(thread_filosofo[i], NULL); //Sincroniza as threads
 	}

 return 0;

}

//Recebe o número do filósfo (0 <= i < n_filosos)
void* filosofo (void * arg){ 
	int i = *((int *) arg);
	while (TRUE){
		pensa(i);
		if(n_porcoes>0){
			pega_garfos(i);
			pthread_mutex_lock(&m); //Garantir o acesso à variavel condicional para somento uma thread por vez
			n_porcoes--;
			pthread_mutex_unlock(&m);
			come(i);
			devolve_garfos(i);
		}else{
			pthread_exit(NULL);
		}
	}

	pthread_exit(NULL);
} 	

void pensa (int i){
	printf ("Filósofo %d está pensando!\n", i);
	sleep(rand() % 10); //Pensa durante um tempo entre 0 e 5 segundos
}

void come (int i){
	printf ("Filósofo %d está comendo!\n", i);
	sleep(rand() % 5); //Come durante um tempo entre 0 e 5 segundos
}

void pega_garfos (int i){
	sem_wait(&mutex); //Entra na região crítica
	estado[i] = FAMINTO;
	tentativa(i); //Tenta pegar os garfos
	sem_post(&mutex); //Sai da região crítica
	sem_wait(&sem[i]); //Bloqueia se não conseguiu pegar os garfos
}

void devolve_garfos(int i) {         
 sem_wait(&mutex); //Entra na região crítica
 estado[i] = PENSANDO;        
 tentativa((i+n_filosofos-1)%n_filosofos); //Verifica se o vizinho da esquerda pode comer agr                
 tentativa((i+1)%n_filosofos); //Verifica se o vizinho da direita pode comer agr               
 sem_post(&mutex); //Sai da região crítica          
}

void tentativa(int i) {
 if (estado[i] == FAMINTO && estado[(i+n_filosofos-1)%n_filosofos] != COMENDO && estado[(i+1)%n_filosofos] != COMENDO) {
    estado[i] = COMENDO;
    sem_post(&sem[i]);
 }
}








