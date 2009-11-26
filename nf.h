/*
 * nf.h
 *
 *  Created on: Nov 24, 2009
 *      Author: hannes
 */
#include "formula.h"

#ifndef NF_H_
#define NF_H_

formula* nnf(formula *form);

formula * distribute_junction(formula* junction,formula *form,junctor innerJunctor );

formula * cross_junctions(formula *j1,formula *j2,junctor innerJunctor);

inline formula* dnf(formula * junction);

inline formula* knf(formula * junction);

#endif /* NF_H_ */
