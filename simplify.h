/*
 * simplify.h
 *
 *  Created on: Nov 25, 2009
 *      Author: hannes
 */

#ifndef SIMPLIFY_H_
#define SIMPLIFY_H_

#include "formula.h"

formula* simplify(formula* form);

int add_formula_to_junction_with_simplification(formula* junction,formula* form);


#endif /* SIMPLIFY_H_ */
