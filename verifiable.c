/*
 * verifiable.c
 *
 *  Created on: Dec 3, 2009
 *      Author: hannes
 */

#include "verifiable.h"

wwbList *new_wwb_list(){
	wwbList *result=(wwbList *)malloc(sizeof(wwbList));
	result->length=0;
	result->first=(wwbListPart *) 0;
	result->last=(wwbListPart *) 0;
	return result;
}

wwbList *clone_wwb_list(wwbList * list){
	wwbListIterator it=wwb_list_iterator_get(list);
	wwbList *result=new_wwb_list();
	while( wwb_list_iterator_valid(it) ){
		wwb_list_append(result,clone_wwb(wwb_list_iterator_wbb(it)));
		it=wwb_list_iterator_next(it);
	}
	return result;
}

void free_wwb_list(wwbList *list){
	wwbListPart *p=list->first,*o;
	if( p ){
		free_wwb(p->wwb);
		o=p->next;
		free(p);
		p=o;
	}
	free(list);
}

void wwb_list_append(wwbList *list,wwb *wwb){
	wwbListPart *part=(wwbListPart *)malloc(sizeof(wwbListPart));
	part->wwb=wwb;
	part->next=(wwbListPart *) 0;
	if( list->first ){
		list->last->next=part;
		list->last=part;
	}else{
		list->first=part;
		list->last=part;
	}
	list->length++;
}

void wwb_list_copy_to(wwbList *from,wwbList *to){
	wwbListIterator it=wwb_list_iterator_get(from);
	while( wwb_list_iterator_valid(it) ){
		wwb_list_append(to,clone_wwb(wwb_list_iterator_wbb(it)));
		it=wwb_list_iterator_next(it);
	}
}

wwbListIterator wwb_list_iterator_get(wwbList *list){
	return list->first;
}

wwbListIterator wwb_list_iterator_next(wwbListIterator it){
	return it->next;
}

wwb *wwb_list_iterator_wbb(wwbListIterator it){
	return it->wwb;
}

int wwb_list_iterator_valid(wwbListIterator it){
	if( it == 0 ){
		return 0;
	}
	return 1;
}
