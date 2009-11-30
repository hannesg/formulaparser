/*
 * dpll.h
 *
 *  Created on: Nov 28, 2009
 *      Author: hannes
 */

#ifndef VERIFIABLE_DPLL_H_
#define VERIFIABLE_DPLL_H_

#include "../formula.h"

#include "../nf.h"

#include "../wwb.h"

int formula_is_verifiable_dpll(formula * form,wwb* w);

#endif /* DPLL_H_ */
