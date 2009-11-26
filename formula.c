#include "formula.h"

formula *new_constant_formula(char value){
	formula *form=(formula*) malloc(sizeof(formula));
	form->type=CONSTANT;
	form->data.constant=(constant) value;
	return form;
}

formula *new_variable_formula(int varNum){
	formula *form=(formula*) malloc(sizeof(formula));
	form->type=VARIABLE;
	form->data.variable=varNum;
	return form;
}

formula *new_negation_formula(formula *inner){
	formula *form=(formula*) malloc(sizeof(formula));
	form->type=NEGATION;
	form->data.negation.inner=inner;
	return form;
}

formula *new_junction_formula(junctor junctor){
	formula *form=(formula*) malloc(sizeof(formula));
	form->type=JUNCTION;
	form->data.junction.part=(junctionPart *) 0;
	form->data.junction.end=(junctionPart *) 0;
	form->data.junction.size=0;
	form->data.junction.junctor=junctor;
	return form;
}

void add_formula_to_junction(formula* junction,formula* form){
	junctionPart *part=(junctionPart *) malloc(sizeof(junctionPart));
	part->formula=form;
	part->next=(junctionPart *) 0;
	if( junction->data.junction.end ){
		junction->data.junction.end->next=part;
	}else{
		junction->data.junction.part=part;
	}
	junction->data.junction.end=part;
	junction->data.junction.size++;
}

junctionIterator junction_iterator_get(formula * form){
	return form->data.junction.part;
}
junctionIterator junction_iterator_next(junctionIterator it){
	return it->next;
}
char junction_iterator_valid(junctionIterator it){
	return !!it;
}
formula * junction_iterator_formula(junctionIterator it){
	return it->formula;
}

void free_formula(formula *form){
	switch( form->type ){
		case CONSTANT:
		case VARIABLE:
			free(form);
			return ;
		case NEGATION:
			free_formula(form->data.negation.inner);
			free(form);
			return ;
		case JUNCTION:
			free_junction_parts(form);
			free(form);
	}
}
void free_junction_parts(formula *form){
	junctionIterator itb,it=junction_iterator_get(form);
	while( junction_iterator_valid(it) ){
		free_formula(junction_iterator_formula(it));
		itb=junction_iterator_next(it);
		free(it);
		it=itb;
	}
}

formula* clone_formula(formula *form){
	junctionIterator it;
	formula* result;
	switch( form->type ){
		case CONSTANT :
			return new_constant_formula(form->data.constant);
		case VARIABLE :
			return new_variable_formula(form->data.variable);
		case NEGATION :
			return new_negation_formula(clone_formula(form->data.negation.inner));
		case JUNCTION :
			result=new_junction_formula(form->data.junction.junctor);
			it=junction_iterator_get(form);
			while( junction_iterator_valid(it) ){
				add_formula_to_junction(
						result,
						clone_formula( junction_iterator_formula(it) )
				);
				it=junction_iterator_next(it);
			}
			return result;
	}
	return new_constant_formula(0);
}

junctor get_junctor(char str){
	return (junctor) str;
}
char is_valid_junctor(junctor j){
	switch( j ){
		case AND:
		case OR:
		case IMPLICATION:
		case EQUIVALENT:
		case XOR:
			return 1;
	}
	return 0;
}
int junction_contains(formula* junction,formula* needle){
	junctionIterator it=junction_iterator_get(junction);
	while( junction_iterator_valid(it) ){
		if( compare_formula(needle,junction_iterator_formula(it)) ){
			return 1;
		}
		it=junction_iterator_next(it);
	}
	return 0;
}

int compare_formula(formula* a,formula* b){
	junctionIterator it;
	if( a->type != b->type ){
		return 0;
	}
	switch( a->type ){
		case CONSTANT:
			return !a->data.constant == !b->data.constant;
		case VARIABLE:
			return a->data.variable == b->data.variable;
		case NEGATION:
			return compare_formula(a->data.negation.inner,b->data.negation.inner);
		case JUNCTION:
			if( a->data.junction.junctor != b->data.junction.junctor ){
				return 0;
			}
			it=junction_iterator_get(a);
			while( junction_iterator_valid(it) ){
				if( !junction_contains(b,junction_iterator_formula(it)) ){
					return 0;
				}
				it=junction_iterator_next(it);
			}
			it=junction_iterator_get(b);
			while( junction_iterator_valid(it) ){
				if( !junction_contains(a,junction_iterator_formula(it)) ){
					return 0;
				}
				it=junction_iterator_next(it);
			}
			return 1;
	}
	return 0;
}

formula * remove_abbreviations(formula * form){
	formula* inner,*result,*temp;
	junctionIterator it;
	switch( form->type ){
		case CONSTANT:
		case VARIABLE:
			return clone_formula(form);
		case NEGATION:
			inner=remove_abbreviations(form->data.negation.inner);
			return new_negation_formula(inner);
		case JUNCTION:
			switch( form->data.junction.junctor ){
				case AND:
				case OR:
					result=new_junction_formula(form->data.junction.junctor);
					it=junction_iterator_get(form);
					while( junction_iterator_valid(it) ){
						add_formula_to_junction(
								result,
								remove_abbreviations(junction_iterator_formula(it)));
						it=junction_iterator_next(it);
					}
					return result;
				case IMPLICATION:
					result=new_junction_formula(OR);
					it=junction_iterator_get(form);
					if( junction_iterator_valid(it) ){
						inner=new_negation_formula(junction_iterator_formula(it));
						add_formula_to_junction(
								result,
								remove_abbreviations(inner));
						free(inner);
						it=junction_iterator_next(it);
					}
					if( junction_iterator_valid(it) ){
						add_formula_to_junction(
								result,
								remove_abbreviations(junction_iterator_formula(it)));
						it=junction_iterator_next(it);
					}
					return result;
				case XOR:
					result=new_junction_formula(OR);
					it=junction_iterator_get(form);
					temp=new_junction_formula(AND);
					if( junction_iterator_valid(it) ){
						inner=new_negation_formula(junction_iterator_formula(it));
						add_formula_to_junction(
								temp,
								remove_abbreviations(inner));
						free(inner);
						it=junction_iterator_next(it);
					}
					if( junction_iterator_valid(it) ){
						add_formula_to_junction(
								temp,
								remove_abbreviations(junction_iterator_formula(it)));
						it=junction_iterator_next(it);
					}
					add_formula_to_junction(result,temp);


					it=junction_iterator_get(form);
					temp=new_junction_formula(AND);
					if( junction_iterator_valid(it) ){
						add_formula_to_junction(
								temp,
								remove_abbreviations(junction_iterator_formula(it)));
						it=junction_iterator_next(it);
					}
					if( junction_iterator_valid(it) ){
						inner=new_negation_formula(junction_iterator_formula(it));
						add_formula_to_junction(
								temp,
								remove_abbreviations(inner));
						free(inner);
					}
					add_formula_to_junction(result,temp);

					return result;
				case EQUIVALENT:
					result=new_junction_formula(OR);
					it=junction_iterator_get(form);
					temp=new_junction_formula(AND);
					if( junction_iterator_valid(it) ){
						add_formula_to_junction(
								temp,
								remove_abbreviations(junction_iterator_formula(it)));
						it=junction_iterator_next(it);
					}
					if( junction_iterator_valid(it) ){
						add_formula_to_junction(
								temp,
								remove_abbreviations(junction_iterator_formula(it)));
						it=junction_iterator_next(it);
					}
					add_formula_to_junction(result,temp);


					it=junction_iterator_get(form);
					temp=new_junction_formula(AND);
					if( junction_iterator_valid(it) ){
						inner=new_negation_formula(junction_iterator_formula(it));
						add_formula_to_junction(
								temp,
								remove_abbreviations(inner));
						free(inner);
						it=junction_iterator_next(it);
					}
					if( junction_iterator_valid(it) ){
						inner=new_negation_formula(junction_iterator_formula(it));
						add_formula_to_junction(
								temp,
								remove_abbreviations(inner));
						free(inner);
					}
					add_formula_to_junction(result,temp);

					return result;
			}
		}
	return new_constant_formula(0);
}
