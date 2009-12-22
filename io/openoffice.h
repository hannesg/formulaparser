/*
 * openoffice.h
 *
 *  Created on: Nov 30, 2009
 *      Author: hannes
 */

#ifndef IO_OPENOFFICE_H_
#define IO_OPENOFFICE_H_

#include "../formula.h"

#include "../io.h"

int formula_required_openoffice_string_length(formula* form);

int formula_to_openoffice_string(formula* form,char* out);

void print_form_openoffice(formula* form);

#endif /* OPENOFFICE_H_ */
