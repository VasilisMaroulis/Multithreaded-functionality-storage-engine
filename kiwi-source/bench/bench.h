#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>   //Bibliothiki gia polunimatismo.


#define KSIZE (16)
#define VSIZE (1000)

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"
long long get_ustime_sec(void);
void _random_key(char *key,int length);


pthread_mutex_t write_m , read_m; //dilwsi mutex tou write kai tou read wste na ypologisoume ta
				  //telika statistika xwris problima otan exoume polla nimata.

pthread_mutex_t tid,tid1,tid2,tid3; //dilwsi mutex tou write kai tou read kai tou writeread

extern int found; //Extren giati allou upoligizetai kai allou tin xrisimopooiw sto bench.c

double totalCostWrite, totalCostRead; //Dilwsi metablhtwn xronou.   

struct data {      // orizo mia domi h opoia periexei tis parametrous pou tha perasoume sthn                              
    long int count;         // To synolo twn leitourgiwn (write - read) pou theloume na ektelesoume
    int r;                  // H metavliti pou dinetai apo tin ekfonisi gia tyxaia kleidia
    int nimata;             // Arithmos twn nimatwn pou theloume na ftiaxoume
};




