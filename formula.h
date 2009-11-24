/*
 * formula.h
 *
 *  Created on: Nov 23, 2009
 *      Author: hannes
 */

#include <malloc.h>

#ifndef FORMULA_H_
#define FORMULA_H_

typedef struct _formula formula;

typedef enum _formulaType {CONSTANT,VARIABLE,NEGATION,JUNCTION} formulaType;

typedef enum _junctor {OR='+',AND='*',EQUIVALENT='=',IMPLICATION='>',XOR='|'} junctor;

typedef int variable;

typedef char constant;

typedef struct _negation{
	formula* inner;
} negation;

typedef struct _junctionPart junctionPart;

typedef junctionPart* junctionIterator;

struct _junctionPart{
	formula* formula;
	junctionPart* next;
};

typedef struct _junction{
	junctionPart* part;
	junctionPart* end;
	char size;
	junctor junctor;
} junction;


struct  _formula{
	union{
		constant constant;
		variable variable;
		negation negation;
		junction junction;
	} data;
	formulaType type;
};

formula *new_constant_formula(char value);

formula *new_variable_formula(int varNum);

formula *new_negation_formula(formula *inner);

formula *new_junction_formula(junctor junctor);

void add_formula_to_junction(formula* junction,formula* form);

junctionIterator junction_iterator_get(formula * form);

junctionIterator junction_iterator_next(junctionIterator it);

char junction_iterator_valid(junctionIterator it);

formula * junction_iterator_formula(junctionIterator it);

void free_formula(formula *form);

formula* clone_formula(formula *form);

junctor get_junctor(char str);

char is_valid_junctor(junctor j);

#endif /* FORMULA_H_ */
