/*
 * nf.c
 *
 *  Created on: Nov 24, 2009
 *      Author: hannes
 */
#include "nf.h"

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
}
