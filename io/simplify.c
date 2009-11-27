/*
 * simplify.c
 *
 *  Created on: Nov 27, 2009
 *      Author: hannes
 */

#include "simplify.h"

void print_simplified(formula *form){
	formula * simplified=simplify(form);
	char * out=(char *)malloc(formula_required_string_length(simplified));
	formula_to_string(simplified,out);
	printf("%s",out);
	free(simplified);
}
