/*
 * simplify.c
 *
 *  Created on: Nov 25, 2009
 *      Author: hannes
 */

#include "simplify.h"

formula* simplify(formula* form){
	junctionIterator it,subIt;
	formula *inner,*temp,*result;
	char needsInvertation=0;
	switch( form->type ){
		case CONSTANT :
		case VARIABLE : // nothing to simplify
			return clone_formula(form);
		case NEGATION :
			inner=simplify(form->data.negation.inner);
			switch( inner->type ){
				case CONSTANT :
					if( inner->data.constant ){
						free_formula(inner);
						return new_constant_formula(0);
					}else{
						free_formula(inner);
						return new_constant_formula(1);
					}
				case VARIABLE :
					return new_negation_formula(inner);
				case NEGATION :
					temp=inner->data.negation.inner;
					free(inner);
					return temp;
				case JUNCTION :
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
								if( add_formula_to_junction_with_simplification(
											result,
											simplify(junction_iterator_formula(it))
									)){
									return result;
								}
								it=junction_iterator_next(it);
							}
							break;
					}

					while( junction_iterator_valid(it) ){
						if( needsInvertation ){
							temp=new_negation_formula(junction_iterator_formula(it));
							if( add_formula_to_junction_with_simplification(
										result,
										simplify(temp)
								) ){
								free(temp);
								return result;
							}
							free(temp);
						}else{
							if( add_formula_to_junction_with_simplification(
										result,
										simplify(junction_iterator_formula(it))
								)){
								return result;
							}
						}
						it=junction_iterator_next(it);
					}
					return result;
			}
		case JUNCTION :
			result=new_junction_formula(form->data.junction.junctor);
			it=junction_iterator_get(form);
			while( junction_iterator_valid(it) ){
				inner=simplify(junction_iterator_formula(it));
				if( inner->type == JUNCTION ){
					if( form->data.junction.junctor == OR || form->data.junction.junctor == AND ){
						if( inner->data.junction.size == 1 || inner->data.junction.junctor == form->data.junction.junctor ){
							//same junctor, so simply add all parts to parent
							subIt=junction_iterator_get(inner);
							while( junction_iterator_valid(subIt) ){
								if( add_formula_to_junction_with_simplification(
											result,
											junction_iterator_formula(subIt)
									) ){
									return result;
								}
								subIt=junction_iterator_next(subIt);
							}
						}else{
							if( add_formula_to_junction_with_simplification(
									result,
									inner
								) ){
								return result;
							}
						}
					}else{
						if( add_formula_to_junction_with_simplification(
								result,
								inner
							) ){
							return result;
						}
					}
				}else{
					if( add_formula_to_junction_with_simplification(
							result,
							inner
						) ){
						return result;
					}
				}
				it=junction_iterator_next(it);
			}
			return result;
	}
	return new_constant_formula(0);
}

int add_formula_to_junction_with_simplification(formula* junction,formula* form){
	formula *temp,*temp2;
	if( junction->type != JUNCTION ){
		return 1;
	}
	if( junction->data.junction.junctor == AND || junction->data.junction.junctor == OR ){
		if( form->type == CONSTANT ){
			if( junction->data.junction.junctor == AND && !form->data.constant){
				free_junction_parts(junction);
				temp=new_constant_formula(0);
				*junction=*temp;
				free(temp);
				return 1;
			}else if( junction->data.junction.junctor == OR && form->data.constant){
				free_junction_parts(junction);
				temp=new_constant_formula(1);
				*junction=*temp;
				free(temp);
				return 1;
			}else{
				return 0;
			}
		}
		if( junction_contains(junction,form) ){
			return 0;
		}
		temp=new_negation_formula(form);
		temp2=simplify(temp);
		if( junction_contains(junction,temp2) ){
			free(temp);
			free_formula(temp2);
			if( junction->data.junction.junctor == AND ){
				free_junction_parts(junction);
				temp=new_constant_formula(0);
				*junction=*temp;
				free(temp);
				return 1;
			}else{
				temp=new_constant_formula(1);
				*junction=*temp;
				free(temp);
				return 1;
			}
		}
		free(temp);
		free_formula(temp2);
	}
	add_formula_to_junction(junction,form);
	return 0;
}
