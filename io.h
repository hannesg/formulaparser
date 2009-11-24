/*
 * serializer.h
 *
 *  Created on: Nov 23, 2009
 *      Author: hannes
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <math.h>
#include <string.h>
#include <malloc.h>

#include "formula.h"

typedef struct _parserResult {
	formula* formula;
	char* rest;
	char error;
} parserResult;

parserResult *string_to_formula_parser(char* str);

int required_string_length(formula* form);

int formula_to_string(formula* form,char *out);

#endif /* SERIALIZER_H_ */
