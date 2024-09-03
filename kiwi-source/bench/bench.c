#include "bench.h"
#include "../engine/db.h"
#define DATAS ("testdb")


DB* db; // dilwsi tou db gia anoigma tis basis.


void _random_key(char *key,int length) {
	int i;
	char salt[36]= "abcdefghijklmnopqrstuvwxyz0123456789";

	for (i = 0; i < length; i++)
		key[i] = salt[rand() % 36];
}

void _print_header(int count)
{
	double index_size = (double)((double)(KSIZE + 8 + 1) * count) / 1048576.0;
	double data_size = (double)((double)(VSIZE + 4) * count) / 1048576.0;

	printf("Keys:\t\t%d bytes each\n", 
			KSIZE);
	printf("Values: \t%d bytes each\n", 
			VSIZE);
	printf("Entries:\t%d\n", 
			count);
	printf("IndexSize:\t%.1f MB (estimated)\n",
			index_size);
	printf("DataSize:\t%.1f MB (estimated)\n",
			data_size);

	printf(LINE1);
}

void _print_environment()
{
	time_t now = time(NULL);

	printf("Date:\t\t%s", 
			(char*)ctime(&now));

	int num_cpus = 0;
	char cpu_type[256] = {0};
	char cache_size[256] = {0};

	FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
	if (cpuinfo) {
		char line[1024] = {0};
		while (fgets(line, sizeof(line), cpuinfo) != NULL) {
			const char* sep = strchr(line, ':');
			if (sep == NULL || strlen(sep) < 10)
				continue;

			char key[1024] = {0};
			char val[1024] = {0};
			strncpy(key, line, sep-1-line);
			strncpy(val, sep+1, strlen(sep)-1);
			if (strcmp("model name", key) == 0) {
				num_cpus++;
				strcpy(cpu_type, val);
			}
			else if (strcmp("cache size", key) == 0)
				strncpy(cache_size, val + 1, strlen(val) - 1);	
		}

		fclose(cpuinfo);
		printf("CPU:\t\t%d * %s", 
				num_cpus, 
				cpu_type);

		printf("CPUCache:\t%s\n", 
				cache_size);
	}
}



void *my_write(void *arg){    //Sunartisi pou tha ektelesoun ta nimata mas.
			      //Oi parametroi pernane me tin xrisi domis.

    struct data *d = (struct data *) arg; //Orizoume tin domi   
    _write_test(d->count, d->r, d->nimata);  //Kaloume tin sunartisi,
    						//pernontas parametrous apo tin domi
						
    return 0;
}



void *my_read(void *arg){  //Sunartisi pou tha ektelesoun ta nimata mas.
		           //Oi parametroi pernane me tin xrisi domis.

	struct data *d = (struct data *) arg; //Orizoume tin domi
	_read_test(d->count, d->r, d->nimata); //Kaloume tin sunartisi,
	                                          //pernontas parametrous apo tin domi
	return 0;
} 


int main(int argc,char** argv)
{
	long int count;
        int nimata; //Dilwsi metablitis gia ta nimata.
	
	pthread_t *tid,*tid1,*tid2,*tid3; //Dilwsi nimatwn.
	
	
	
	
	pthread_mutex_init(&write_m,NULL); //Arxikopoisi tou mutex
	pthread_mutex_init(&read_m,NULL);
	
	
	
	srand(time(NULL));
	
	//db = db_open(DATAS); //Anoigma basis.
	
	if (argc < 4) {
		fprintf(stderr,"Usage: db-bench <write | read> <count> <nimata>\n");
		exit(1);
	}
	
	nimata = atoi(argv[3]); //O arithmos twn nimatwn oi 3H parametro
	tid = (pthread_t*) malloc(nimata *sizeof(pthread_t));//Dunamiki deusmeufsi mnimis
	tid1 = (pthread_t*) malloc(nimata *sizeof(pthread_t));
	tid2 = (pthread_t*) malloc(nimata *sizeof(pthread_t));
	tid3 = (pthread_t*) malloc(nimata *sizeof(pthread_t));
	
	if (strcmp(argv[1], "write") == 0) {

		int r = 0;
		totalCostWrite = 0.0; //metabliti gia ton sunoliko xrono
		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		
		db = db_open(DATAS); //anoigw tin basi
		if (argc > 4)
			r = 1;
			
		struct data data1 = {count,r,nimata}; //Dinoume times stin domi.
		
                for( int i = 0; i < nimata; i++){
			pthread_create(&tid[i],NULL, my_write, (void *)&data1); //Dimiourgia nimatwn.
		}
		for( int i = 0; i < nimata; i++){
			pthread_join(tid[i],NULL); //Gia kathe nima pou ftiaxnw perimenw mexri na 
						    //teleiwsoun ola.
		}
		 
	        printf("|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n"
		,count, (double)(totalCostWrite / count),(double)(count / totalCostWrite) ,totalCostWrite);		
	} 
	
	
	
	else if (strcmp(argv[1], "read") == 0) {
		int r = 0;
	        totalCostRead = 0.0; //metabliti gia ton sunoliko xrono
		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		
		db = db_open(DATAS); //anoigw tin basi
		if (argc > 4)
			r = 1;
		struct data data2 = {count,r,nimata}; //Dinoume times stin domi.
		for( int i = 0; i < nimata; i++){
			pthread_create(&tid1[i],NULL, my_read, (void *)&data2); //Dimiourgia nimatwn.
		}
		for( int i = 0; i < nimata; i++){
			pthread_join(tid1[i],NULL); //Gia kathe nima pou ftiaxnw perimenw mexri na,
		                                     //teleiwsoun ola. 
		}
		
		
		printf("|Random-Read	(done:%ld, found:%d): %.6f sec/op; %.1f reads /sec(estimated); cost:%.3f(sec)\n",     
		count, found,(double)(totalCostRead / count),(double)(count / totalCostRead),totalCostRead);
	}
	
	
	else if(strcmp(argv[1], "writeread") == 0){
		int r = 0;
		int count1 =0;
		int count2 = 0; //Dilwsi metablhtwn gia na ypolgisw tis leitoyrgies.
		
		if(argc < 5){
			fprintf(stderr,"Usage: db-bench <writeread> <count> <threads> <pososto> <random>\n");
			exit(1);		
		}
		int pososto = atoi(argv[4]);
		db = db_open(DATAS); //anoigw tin basi
		
		count1 =  (atoi(argv[2])*(pososto*0.01));
		count2 =  (atoi(argv[2])*(1 - pososto*0.01)); 
		
	
		_print_environment();
		if (argc == 6)
			r = 1;		
			
		struct data data3 = {count1,r,nimata}; //Dinoume times stin domi.
		struct data data4 = {count2,r,nimata}; //Dinoume times stin domi.
		for( int i = 0; i < nimata; i++){
	
			//Dimiourgia nimatwn pou tha kalesoun tin my_write.
			pthread_create(&tid2[i],NULL, my_write, (void *)&data3);
			
			//Dimiourgia nimatwn pou tha kalesoun tin my_read.
			pthread_create(&tid3[i],NULL, my_read, (void *)&data4);	
		}
		for( int i = 0; i < nimata; i++){
			pthread_join(tid2[i],NULL); //Gia kathe nima pou ftiaxnw perimenw mexri na,
		                                     //teleiwsoun ola. 
						     
			pthread_join(tid3[i],NULL); //Gia kathe nima pou ftiaxnw perimenw na teleiwsoun.
		}
		
		// emfanizo ta apotelesmata gia count1 write leitourgies
		printf("|Random-Write	(done:%d): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n"      
		,count1, (double)(totalCostWrite / count1)
		,(double)(count1 / totalCostWrite),totalCostWrite);
		
		
		// emfanizo ta apotelesmata gia count2 read leitourgies
		printf("|Random-Read	(done:%d, found:%d): %.6f sec/op; %.1f reads /sec(estimated); cost:%.3f(sec)\n",     
		count2, found,
		(double)(totalCostRead / count2),
		(double)(count2 / totalCostRead),
		totalCostRead);
	}
	
	
	
	
	
	
	
	 else {
		fprintf(stderr,"Usage: db-bench <write | read | writeread> <count> <threads> <pososto> <random>\n");
		exit(1);
	}
	free(tid); //Apodeusmeusi
	free(tid1);
	free(tid2);
	free(tid3);
	db_close(db); //Kleinw tin basi.

	return 1;
}
