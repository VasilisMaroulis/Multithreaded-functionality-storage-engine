#include <string.h>
#include "../engine/db.h"
#include "../engine/variant.h"
#include "bench.h"
#define DATAS ("testdb")

int  found =0 ; //arxikopoisi tou found , dhlwmeno sto bench.h
DB *db;

void _write_test(long int count, int r,int nimata)
{
	int i ,x;
	double cost;
	long long start,end;
	Variant sk, sv;
	
	char key[KSIZE + 1];
	char val[VSIZE + 1];
	char sbuf[1024];

	memset(key, 0, KSIZE + 1);
	memset(val, 0, VSIZE + 1);
	memset(sbuf, 0, 1024);
	
	x = count / nimata; //Moirazw isotima tiw leitourgies se katse nima.
			    
	start = get_ustime_sec();
	for (i = 0; i < x; i++) {
		if (r==1)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);

		sk.length = KSIZE;
		sk.mem = key;
		sv.length = VSIZE;
		sv.mem = val;

		db_add(db, &sk, &sv);
		if ((i % 10000) == 0) {
			fprintf(stderr,"random write finished %d ops%30s\r", i,"");
			fflush(stderr);
		}
	}
	end = get_ustime_sec();
	cost = end -start;
	
	pthread_mutex_lock(&write_m); //kleidwnw to mutex
        totalCostWrite = totalCostWrite + cost; //Prosthetw sto sunoliko kostos to xronou ektelis
						//tou kathe nimatos gia na kratisw to apotelesma.
        pthread_mutex_unlock(&write_m); //xekleidwnw to  mutex
	printf(LINE); 	
}

void _read_test(long int count, int r, int nimata)
{
	int i,x;
	int ret;
	double cost;
	long long start,end;
	Variant sk, sv;
	
	char key[KSIZE + 1];

	x = count / nimata; //Moirazw isotima tiw leitourgies se katse nima.

	start = get_ustime_sec();

	for (i = 0; i < x; i++) {
		memset(key, 0, KSIZE + 1);

		/* if you want to test random write, use the following */
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d searching %s\n", i, key);
		sk.length = KSIZE;
		sk.mem = key;
		ret = db_get(db, &sk, &sv);
		if (ret) {
			//db_free_data(sv.mem);
			found++;
		} else {
			INFO("not found key#%s", 
					sk.mem);
    	}

		if ((i % 10000) == 0) {
			fprintf(stderr,"random read finished %d ops%30s\r",i, "");
			fflush(stderr);
		}
	}
	end = get_ustime_sec();
	cost = end - start;
	
        pthread_mutex_lock(&read_m); //kleidwnw to mutex
        totalCostRead = totalCostRead + cost;//Prosthetw sto sunoliko kostos to xronou ektelis
                                             //tou kathe nimatos gia na kratisw to apotelesma.
        pthread_mutex_unlock(&read_m);  //xekleidwnw to  mutex	     
	printf(LINE);
}
