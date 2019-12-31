#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "omp.h"

struct tablo {
  int * tab;
  int size;
};

void printArray(struct tablo * tmp) {
  printf("---- Array of size %d ---- \n", tmp->size);
  int size = tmp->size;
  int i;
  for (i = 0; i < size; ++i) {
    printf("%i ", tmp->tab[i]);
  }
  printf("\n");
}

struct tablo * allocateTablo(int size) {
  struct tablo * tmp = malloc(sizeof(struct tablo));
  tmp->size = size;
  tmp->tab = malloc(size*sizeof(int));
  return tmp;
}

void montee(struct tablo * source, struct tablo * destination) {
	int startposition = destination->size - source->size - 1;
	int p = log(source->size)/log(2) -1;
	for(int i =0 ;i<source->size;i++){
		destination->tab[startposition + i] = source->tab[i];
	}
	for(int i = 0 ; i<(p+1) ;i++){
		int finishposition = pow(2,p-i+1)-1;
		omp_set_num_threads(2);
		#pragma omp parallel for
		for(int j = (pow(2, p-i)-1); j < finishposition ; j++)
		{
			destination->tab[j] = destination->tab[2*j+1] + destination->tab[2*j+2];
		}
	}
}

void maxmontee(struct tablo * source, struct tablo * destination){
	int startposition = destination->size - source->size - 1;
	int p = log(source->size)/log(2) -1;
	for(int i =0 ;i<source->size;i++){
		destination->tab[startposition + i] = source->tab[i];
	}
	for(int i = 0 ; i<(p+1) ;i++){
		int finishposition = pow(2,p-i+1)-1;
		omp_set_num_threads(2);
		#pragma omp parallel for
		for(int j = (pow(2, p-i)-1);j < finishposition ; j++){
			if(destination->tab[2*j+1] < destination->tab[2*j+2])destination->tab[j] = destination->tab[2*j+2];
			else	destination->tab[j] = destination->tab[2*j+1];
		}
	}
}

void descente(struct tablo * a, struct tablo * b) {
	int p = log(a->size)/log(2);
	for(int i=0; i<p;i++){
		int finishposition = pow(2,i+1)-1;
		omp_set_num_threads(2);
		#pragma omp parallel for
		for(int j=(pow(2,i)-1);j < finishposition;j++){
			if(j==0)b->tab[j]=0;
			else if(j%2==1)b->tab[j]=b->tab[(j-1)/2];
			else b->tab[j]=b->tab[(j-2)/2] + a->tab[j-1];
		}
	}
}

void maxdescente(struct tablo * a, struct tablo * b){
	int p = log(a->size)/log(2);
	for(int i=0; i<p;i++){
		int finishposition = pow(2,i+1)-1;
		omp_set_num_threads(2);
		#pragma omp parallel for
		for(int j=(pow(2,i)-1);j < finishposition ; j++){
			if(j==0)b->tab[0]=a->tab[a->size/2-1];
			else if(j%2==1)b->tab[j]=b->tab[(j-1)/2];
			else {
				if(a->tab[j-1] > b->tab[(j-2)/2])b->tab[j]= a->tab[j-1];
				else b->tab[j]= b->tab[(j-2)/2];
			}
		}
	}
}

void final(struct tablo * a, struct tablo *b) {
	// #pragma omp parallel for
	for(int i=(a->size/2-1);i<(a->size);i++){
		b->tab[i-(a->size/2-1)]=b->tab[i]+a->tab[i];
	}
}

void maxfinal(struct tablo * a, struct tablo *b){
	// #pragma omp parallel for
	for(int i=(a->size/2-1);i<(a->size-1);i++){
		if(b->tab[i] < a->tab[i])b->tab[i-(a->size/2-1)]=a->tab[i];
		else b->tab[i-(a->size/2-1)] = b->tab[i];
	}
}

void reversesource(struct tablo * reversesource, struct tablo *source){
  omp_set_num_threads(2);
  #pragma omp parallel for
  for(int i=0; i<source->size ; i++){
    reversesource->tab[i] = source->tab[source->size-i-1];
  }
}

void reverseresulat(struct tablo * reversesource, struct tablo *source){
  omp_set_num_threads(2);
  #pragma omp parallel for
  for(int i=0; i<source->size/2 ; i++){
    reversesource->tab[i] = source->tab[source->size/2-i-1];
  }
}

void calculatearrayM(struct tablo * m,struct tablo * pmax,struct tablo * smax,struct tablo * psum,struct tablo * ssum,struct tablo * source){
  omp_set_num_threads(2);
  #pragma omp parallel for
  for(int i=0;i<source->size;i++){
    m->tab[i]=pmax->tab[i]+smax->tab[i]-psum->tab[i]-ssum->tab[i]+source->tab[i];
  }
}

// void generateArray(struct tablo * s) {
//   s->size=16;
//   s->tab=malloc(s->size*sizeof(int));
//   s->tab[0]=3;
//   s->tab[1]=2;
//   s->tab[2]=-7;
//   s->tab[3]=11;
//   s->tab[4]=10;
//   s->tab[5]=-6;
//   s->tab[6]=4;
//   s->tab[7]=9;
//   s->tab[8]=-6;
//   s->tab[9]=1;
//   s->tab[10]=-2;
//   s->tab[11]=-3;
//   s->tab[12]=4;
//   s->tab[13]=-3;
//   s->tab[14]=0;
//   s->tab[15]=2;
// }

int main(int argc, char **argv) {
  //---------------------------------------------------------read data
  FILE * fp;
  int len=0;
  int x=0;
  fp = fopen(argv[1], "r");
  while(fscanf(fp,"%d",&x)>0)
  {
    len+=1;
  }
  fclose(fp);
  struct tablo * source = allocateTablo(len);
  fp = fopen(argv[1], "r");
  int j=0;
  while(fscanf(fp,"%d",&x)>0)
  {
    source->tab[j]=x;
    j+=1;
  }
  fclose(fp);
  //---------------------------------------------------------calculate psum
  struct tablo * a = allocateTablo(source->size*2);
  struct tablo * psum = allocateTablo(source->size*2);
  montee(source, a);
  descente(a, psum);
  final(a,psum);
  //---------------------------------------------------------calculate ssum
  struct tablo * reversedsource = allocateTablo(source->size);
  struct tablo * a1 = allocateTablo(source->size*2);
  struct tablo * b1 = allocateTablo(source->size*2);
  struct tablo * ssum = allocateTablo(source->size);
  reversesource(reversedsource,source);
  montee(reversedsource, a1);
  descente(a1, b1);
  final(a1,b1);
  reverseresulat(ssum,b1);
  //---------------------------------------------------------calculate pmax
  struct tablo * a2 = allocateTablo(source->size*2);
  struct tablo * pmax = allocateTablo(source->size*2);
  maxmontee(ssum, a2);
  maxdescente(a2, pmax);
  maxfinal(a2,pmax);
  //---------------------------------------------------------calculate smax
  struct tablo * reversedsource2 = allocateTablo(source->size);
  struct tablo * a3 = allocateTablo(source->size*2);
  struct tablo * b3 = allocateTablo(source->size*2);
  struct tablo * smax = allocateTablo(source->size);
  reverseresulat(reversedsource2,psum);
  maxmontee(reversedsource2, a3);
  maxdescente(a3, b3);
  maxfinal(a3,b3);
  reverseresulat(smax,b3);
  //---------------------------------------------------------calculate array m
  struct tablo * m = allocateTablo(source->size);
  calculatearrayM(m, pmax,smax,psum,ssum,source);
  //---------------------------------------------------------print resulat
  struct tablo * r = allocateTablo(source->size*2);
  maxmontee(m, r);
  printf("%d ", r->tab[0]);
  for(int i=0; i<source->size;i++){
  	if(m->tab[i]==r->tab[0])printf("%d ", source->tab[i]);
  }
  printf("\n");
  return 0;
}