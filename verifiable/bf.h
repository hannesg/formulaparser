/*
 * bf.h
 *
 *  Created on: Dec 1, 2009
 *      Author: hannes
 */

#ifndef VERIFIABLE_BF_H_
#define VERIFIABLE_BF_H_

#include <pthread.h>
#include <semaphore.h>

#include "../formula.h"
#include "../wwb.h"
#include "../verifiable.h"

#define NUM_THREADS 2

typedef struct _bruteForceParams {
	formula *form;
	wwb *baseWwb;
	int findAll;
	wwbList *wwbs;

} bruteForceParams;

void* is_verifiable_bf_thread(void *threadParams);

int is_verifiable_bf(formula *form,wwb *baseWwb);


#endif /* BF_H_ */
