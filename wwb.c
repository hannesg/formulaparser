/*
 * wwb.c
 *
 *  Created on: Nov 27, 2009
 *      Author: hannes
 */

#include "wwb.h"

wwb * new_wwb(){
	wwb *w=(wwb *) malloc(sizeof(wwb));
	w->flags=FREE;
	w->left=(wwb *)0;
	w->right=(wwb *)0;
	w->value=0;
	w->variable=0;
	return w;
}

wwb * clone_wwb(wwb *w){
	wwb* result;
	if( !w ){
		return w;
	}
	result=(wwb *) malloc(sizeof(wwb));
	result->flags=w->flags;
	result->value=w->value;
	result->variable=w->variable;
	result->left=clone_wwb(w->left);
	result->right=clone_wwb(w->right);
	return result;
}

void free_wwb(wwb *w){
	if( !w ){
		return ;
	}
	free_wwb(w->left);
	free_wwb(w->right);
	free(w);
}


inline int is_wwb(wwb *w){
	if( !w ) return 0;
	if( w->flags&FREE ) return 0;
	return 1;
}

inline int wwb_is_locked(wwb *w){
	return w->flags&LOCKED;
}

int wwb_get(wwb * wwb,variable var){
	while( wwb ){
		if( wwb->variable == var ){
			return wwb->value;
		}else if( wwb->variable > var ){
			wwb=wwb->left;
		}else{
			wwb=wwb->right;
		}
	}
	return -1;
}

void wwb_set_with_lock(wwb * w,variable var,int value,int lock){
	while( w ){
		if( w->flags & FREE ){
			w->flags&=~FREE;
			w->variable=var;
		}
		if( w->variable == var ){
			if( lock ){
				w->flags|=LOCKED;
			}
			w->value=value;
			return ;
		}else if( w->variable > var ){
			if( !w->left ){
				w->left=new_wwb();
			}
			w=w->left;
		}else{
			if( !w->right ){
				w->right=new_wwb();
			}
			w=w->right;
		}
	}
}

inline void wwb_set(wwb * w,variable var,int value){
	return wwb_set_with_lock(w,var,value,0);
}
inline void wwb_set_and_lock(wwb * w,variable var,int value){
	return wwb_set_with_lock(w,var,value,1);
}

void add_variables_from_formula(formula * form,wwb * wwb){
	junctionIterator it;
	switch( form->type ){
		case CONSTANT :
			return ;
		case VARIABLE :
			wwb_set(wwb,form->data.variable,0);
			return ;
		case NEGATION:
			add_variables_from_formula(form->data.negation.inner,wwb);
			return ;
		case JUNCTION:
			it=junction_iterator_get(form);
			while( junction_iterator_valid(it) ){
				add_variables_from_formula(junction_iterator_formula(it),wwb);
				it=junction_iterator_next(it);
			}
	}
}

wwb * wwb_from_formula(formula * form){
	wwb * w=new_wwb();
	add_variables_from_formula(form,w);
	return w;
}


int formula_evaluate(formula * form,wwb * wwb){
	junctionIterator it;
	int result;
	switch( form->type ){
		case CONSTANT :
			return (int) CONSTANT;
		case VARIABLE :
			result=wwb_get(wwb,form->data.variable);
			return result;
		case NEGATION:
			return !formula_evaluate(form->data.negation.inner,wwb);
		case JUNCTION:
			it=junction_iterator_get(form);
			switch( form->data.junction.junctor ){
				case OR :
					while( junction_iterator_valid(it) ){
						if( formula_evaluate(junction_iterator_formula(it),wwb) ){
							return 1;
						}
						it=junction_iterator_next(it);
					}
					return 0;
				case AND :
					while( junction_iterator_valid(it) ){
						if( !formula_evaluate(junction_iterator_formula(it),wwb) ){
							return 0;
						}
						it=junction_iterator_next(it);
					}
					return 1;
			}
	}
	return -1;
}

formula* formula_evaluate_partially(formula * form,wwb * wwb){
	junctionIterator it;
	formula *result,*innerResult;
	int value;
	switch( form->type ){
		case CONSTANT :
			return clone_formula(form);
		case VARIABLE :
			value=wwb_get(wwb,form->data.variable);
			if( value == -1 ){
				return clone_formula(form);
			}else{
				return new_constant_formula((char) !!value);
			}
		case NEGATION:
			innerResult=formula_evaluate_partially(form->data.negation.inner,wwb);
			if( innerResult->type == CONSTANT ){
				result=new_constant_formula(!innerResult->data.constant);
				free_formula(innerResult);
				return result;
			}
			return new_negation_formula(innerResult);
		case JUNCTION:
			it=junction_iterator_get(form);
			result=new_junction_formula(form->data.junction.junctor);
			switch( form->data.junction.junctor ){
				case OR :
					while( junction_iterator_valid(it) ){
						innerResult=formula_evaluate_partially(junction_iterator_formula(it),wwb);
						if( innerResult->type == CONSTANT ){
							if( innerResult->data.constant ){
								free_formula(result);
								return innerResult;
							}
						}else{
							add_formula_to_junction(result,innerResult);
						}
						it=junction_iterator_next(it);
					}
				case AND :
					while( junction_iterator_valid(it) ){
						innerResult=formula_evaluate_partially(junction_iterator_formula(it),wwb);
						if( innerResult->type == CONSTANT ){
							if( !innerResult->data.constant ){
								free_formula(result);
								return innerResult;
							}
						}else{
							add_formula_to_junction(result,innerResult);
						}
						it=junction_iterator_next(it);
					}
			}
			if( result->data.junction.size == 0 ){
				free_formula(result);
				if( form->data.junction.junctor==AND ){
					return new_constant_formula(1);
				}else{
					return new_constant_formula(0);
				}
			}
			return result;
	}
}

wwb* wwb_successor(wwb *w,int *carry){
	wwb* result=new_wwb();
	int subCarry=0;
	if( !is_wwb(w) ){
		*carry=1;
		if( !w ){
			free(result);
			return w;
		}
		return result;
	}
	result->flags=w->flags;
	result->left=wwb_successor(w->left,&subCarry);
	if( subCarry ){
		if( wwb_is_locked(w) ){
			result->right=wwb_successor(w->right,carry);
			return result;
		}
		if( w->value ){
			wwb_set(result,w->variable,0);
			result->right=wwb_successor(w->right,carry);
			return result;
		}else{
			wwb_set(result,w->variable,1);
			result->right=clone_wwb(w->right);
			*carry=0;
			return result;
		}
	}
	wwb_set(result,w->variable,w->value);
	result->right=clone_wwb(w->right);
	*carry=0;
	return result;
}

void wwb_copy_to(wwb* from,wwb* to){
	if( is_wwb(from) ){
		wwb_set_with_lock(to,from->variable,from->value,wwb_is_locked(from));
		wwb_copy_to(from->left,to);
		wwb_copy_to(from->right,to);
	}
}
