/*
 * bf.c
 *
 *  Created on: Dec 1, 2009
 *      Author: hannes
 */

#include <stdio.h>
#include <unistd.h>


#include "bf.h"

void * is_verifiable_bf_thread(void *threadParams){

	bruteForceParams *params=(bruteForceParams *) threadParams;
	wwb *wwb,*oldWwb;
	wwbList *list=params->wwbs;
	formula *form=params->form;
	int carry=0,findAll=params->findAll;

	wwb=clone_wwb(params->baseWwb);
	while( !carry ){
		usleep(1);
		oldWwb=wwb;
		if( formula_evaluate(form,wwb) ){
			printf("found!\n");
			if( findAll ){
				wwb_list_append(list,clone_wwb(wwb));
			}else{
				free_wwb(wwb);
				return NULL;
			}
		}
		wwb=wwb_successor(oldWwb,&carry);
		free_wwb(oldWwb);
	}
	free_wwb(wwb);

	return NULL;
}

bruteForceParams *new_brute_force_params(formula *form,wwb *baseWwb,int findAll){
	bruteForceParams *result=(bruteForceParams *) malloc(sizeof(bruteForceParams));
	result->form=form;
	result->baseWwb=baseWwb;
	result->findAll=findAll;
	result->wwbs=new_wwb_list();
	return result;
}

int is_verifiable_bf(formula *form,wwb *baseWwb){
	pthread_t *thread;
	bruteForceParams **params;
	int num=0,vars=1;
	wwb *extract;
	int threads=(int) pow(2,vars);

	thread=(pthread_t *)malloc(sizeof(pthread_t)*threads);
	params=(bruteForceParams **)malloc(sizeof(bruteForceParams*)*threads);


	while( num < threads ){
		extract=clone_wwb(baseWwb);
		wwb_set_and_lock(extract,num%2,num);
		params[num]=new_brute_force_params(form,extract,1);
		pthread_create(thread+num,NULL,&is_verifiable_bf_thread,params[num]);
		num++;
	}


	usleep(1000);

	num=0;

	while( num < threads ){
		pthread_join(thread[num],NULL);
		num++;
	}

	return 0;
}
