/*
 * nf.c
 *
 *  Created on: Nov 24, 2009
 *      Author: hannes
 */
#include "nf.h"
#include "io.h"

formula* nnf(formula *form){
	junctionIterator it;
	formula *inner,*temp,*result;
	char needsInvertation=0;
	switch( form->type ){
		case CONSTANT :
		case VARIABLE :
			return clone_formula(form);
		case NEGATION :
			switch( form->data.negation.inner->type ){
				case CONSTANT :
					inner=form->data.negation.inner;
					if( inner->data.constant ){
						return new_constant_formula(0);
					}else{
						return new_constant_formula(1);
					}
				case VARIABLE :
					return clone_formula(form);
				case NEGATION :
					inner=form->data.negation.inner;
					return nnf(inner->data.negation.inner);
				case JUNCTION :
					inner=form->data.negation.inner;
					result=new_junction_formula(0);
					it=junction_iterator_get(inner);
					switch( inner->data.junction.junctor ){
						case AND :
							needsInvertation=1;
							result->data.junction.junctor=OR;
							break;
						case OR :
							needsInvertation=1;
							result->data.junction.junctor=AND;
							break;
						case XOR :
							needsInvertation=0;
							result->data.junction.junctor=EQUIVALENT;
							break;
						case EQUIVALENT :
							needsInvertation=0;
							result->data.junction.junctor=XOR;
							break;
						case IMPLICATION :
							needsInvertation=1;
							result->data.junction.junctor=AND;
							if( junction_iterator_valid(it) ){
								add_formula_to_junction(
										result,
										nnf( junction_iterator_formula(it) )
								);
								it=junction_iterator_next(it);
							}
							break;
					}

					while( junction_iterator_valid(it) ){
						if( needsInvertation ){
							temp=new_negation_formula(junction_iterator_formula(it));
							add_formula_to_junction(
									result,
									nnf( temp )
							);
							free(temp);
						}else{
							add_formula_to_junction(
									result,
									nnf( junction_iterator_formula(it) )
							);
						}
						it=junction_iterator_next(it);
					}
					return result;
			}
		case JUNCTION :
			result=new_junction_formula(form->data.junction.junctor);
			it=junction_iterator_get(form);
			while( junction_iterator_valid(it) ){
				add_formula_to_junction(
						result,
						nnf( junction_iterator_formula(it) )
				);
				it=junction_iterator_next(it);
			}
			return result;
	}
	return new_constant_formula(0);
}
int is_literal(formula* item){
	switch( item->type ){
		case CONSTANT:
		case VARIABLE:
			return 1;
		case NEGATION:
			if(item->data.negation.inner->type == CONSTANT || item->data.negation.inner->type == VARIABLE){
				return 1;
			}
			return 0;
		case JUNCTION:
			return 0;
		}
	return 0;
}
int junction_contains_only_literals(formula* junction){
	junctionIterator it=junction_iterator_get(junction);
	formula *item;
	while( junction_iterator_valid(it) ){
		item=junction_iterator_formula(it);
		if( !is_literal(item) ){
			return 0;
		}
		it=junction_iterator_next(it);
	}
	return 1;
}

void junction_append(formula* junction,formula* append){
	junctionIterator it;
	if( append->type != JUNCTION ){
		add_formula_to_junction(
				junction,
				clone_formula(append)
		);
		return ;
	}

	it=junction_iterator_get(append);
	while( junction_iterator_valid(it) ){
		add_formula_to_junction(
				junction,
				clone_formula(junction_iterator_formula(it))
		);
		it=junction_iterator_next(it);
	}
}

formula * cross_junctions(formula *j1,formula *j2,junctor innerJunctor){
	formula *result,*inner,*temp,*temp2;
	junctionIterator it1,it2;
	if( j2->data.junction.size == 0){
		return clone_formula(j1);
	}
	if( j1->data.junction.size == 0){
		return clone_formula(j2);
	}
	if( j2->data.junction.size == 1 ){
		return distribute_junction(j1,j2->data.junction.part->formula,innerJunctor);
	}
	if( j1->data.junction.size == 1 ){
		return distribute_junction(j2,j1->data.junction.part->formula,innerJunctor);
	}
	result=new_junction_formula(j1->data.junction.junctor);
	it1=junction_iterator_get(j1);
	while( junction_iterator_valid(it1) ){
		inner=junction_iterator_formula(it1);
		if( inner->type == JUNCTION && inner->data.junction.junctor==innerJunctor){
			temp=clone_formula(inner);
		}else{
			temp=new_junction_formula(innerJunctor);
			add_formula_to_junction(temp,clone_formula(inner));
		}
		it2=junction_iterator_get(j2);
		while( junction_iterator_valid(it2) ){
			inner=junction_iterator_formula(it2);
			temp2=clone_formula(temp);
			if( inner->type == JUNCTION && inner->data.junction.junctor==innerJunctor){// append
				junction_append(temp2,inner);
			}else{
				add_formula_to_junction(temp2,clone_formula(inner));
			}
			add_formula_to_junction(result,temp2);
			it2=junction_iterator_next(it2);
		}
		free_formula(temp);
		it1=junction_iterator_next(it1);
	}
	return result;
}

formula * distribute_junction(formula* junction,formula *form,junctor innerJunctor ){
	junctionIterator it=junction_iterator_get(junction);
	formula *inner,*result,*temp;
	result=new_junction_formula(junction->data.junction.junctor);
	while( junction_iterator_valid(it) ){
		inner=junction_iterator_formula(it);
		if( inner->type == JUNCTION && inner->data.junction.junctor==innerJunctor){
			temp=clone_formula(inner);
			add_formula_to_junction(temp,clone_formula(form));
		}else{
			temp=new_junction_formula(innerJunctor);
			add_formula_to_junction(temp,clone_formula(inner));
			add_formula_to_junction(temp,clone_formula(form));
		}
		add_formula_to_junction(result,temp);
		it=junction_iterator_next(it);
	}
	return result;
}

formula *nf(formula *form,junctor junctor){
	junctionIterator it;
	formula *temp,*result,*oldResult;
	form=nnf(form);
	switch( form->type ){
		case CONSTANT :
		case VARIABLE :
		case NEGATION :
			result=new_junction_formula(junctor);
			add_formula_to_junction(
					result,
					form
			);
			return result;
		case JUNCTION :
			if( form->data.junction.junctor == junctor ){ // nice!
				result=new_junction_formula(junctor);
				it=junction_iterator_get(form);
				while( junction_iterator_valid(it) ){
					temp=nf( junction_iterator_formula(it),junctor  );
					junction_append(result,temp);
					free_formula(temp);
					it=junction_iterator_next(it);
				}
				free_formula(form);
				return result;
			}else if( junction_contains_only_literals(form) ){ // nice too!
				result=new_junction_formula(junctor);
				add_formula_to_junction(
						result,
						form
				);
				return result;
			}else{// oh no!
				it=junction_iterator_get(form);
				result=new_junction_formula(junctor);
				while( junction_iterator_valid(it) ){
					temp=nf( junction_iterator_formula(it),junctor );
					oldResult=result;
					result=cross_junctions(result,temp,junctor==AND?OR:AND);
					free_formula(temp);
					free_formula(oldResult);
					it=junction_iterator_next(it);
				}
				free_formula(form);
				return result;
			}
	}
	return new_constant_formula(0);
}
inline formula* dnf(formula * junction){
	return nf(junction,OR);
}
inline formula* knf(formula * junction){
	return nf(junction,AND);
}
