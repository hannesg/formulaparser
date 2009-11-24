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
	junctionIterator it,itb;
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
			it=junction_iterator_get(form);
			while( junction_iterator_valid(it) ){
				free_formula(junction_iterator_formula(it));
				itb=junction_iterator_next(it);
				free(it);
				it=itb;
			}
			free(form);
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
char test_formula(formula* form){

}
