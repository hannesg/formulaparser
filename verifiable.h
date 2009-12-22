/*
 * verifiable.h
 *
 *  Created on: Dec 3, 2009
 *      Author: hannes
 */

#ifndef VERIFIABLE_H_
#define VERIFIABLE_H_

#include "wwb.h"

typedef struct _wwbListPart wwbListPart;

struct _wwbListPart {
	wwb *wwb;
	wwbListPart *next;
};

typedef struct _wwbList {
	wwbListPart *first;
	wwbListPart *last;
	int length;
} wwbList;

typedef wwbListPart * wwbListIterator;

wwbList *new_wwb_list();

wwbList *clone_wwb_list(wwbList *list);

void free_wwb_list(wwbList *list);

void wwb_list_append(wwbList *list,wwb *wwb);

void wwb_list_copy_to(wwbList *from,wwbList *to);

wwbListIterator wwb_list_iterator_get(wwbList *list);

wwbListIterator wwb_list_iterator_next(wwbListIterator it);

wwb *wwb_list_iterator_wbb(wwbListIterator it);

int wwb_list_iterator_valid(wwbListIterator it);


#endif /* VERIFIABLE_H_ */
