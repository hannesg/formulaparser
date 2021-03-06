/*
 * wwb.h
 *
 *  Created on: Nov 27, 2009
 *      Author: hannes
 */

#ifndef WWB_H_
#define WWB_H_

#include <malloc.h>

#include "formula.h"

typedef struct _wwb wwb;

typedef enum _wwbFlags {FREE=1,LOCKED=2} wwbFlags;

struct _wwb {
	variable variable;
	wwb* left;
	wwb* right;
	int flags;
	int value;
};

wwb * new_wwb();

wwb * clone_wwb(wwb *w);

void free_wwb(wwb *w);

inline int is_wwb(wwb *w);

int wwb_get(wwb * wwb,variable var);

inline void wwb_set(wwb * w,variable var,int value);

inline void wwb_set_and_lock(wwb * w,variable var,int value);

wwb * wwb_from_formula(formula * form);

int formula_evaluate(formula * form,wwb * wwb);

formula* formula_evaluate_partially(formula * form,wwb * wwb);

wwb* wwb_successor(wwb *w,int *carry);

void wwb_copy_to(wwb* from,wwb* to);

void wwb_copy_to_and_lock(wwb* from,wwb* to);

#endif /* WWB_H_ */
