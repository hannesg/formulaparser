/*
 * wwb.h
 *
 *  Created on: Nov 27, 2009
 *      Author: hannes
 */

#ifndef IO_WWB_H_
#define IO_WWB_H_

#include <math.h>
#include <string.h>
#include <malloc.h>

#include "../io.h"
#include "../wwb.h"


int wwb_to_string(wwb* ww,char* out);

int wwb_required_string_length(wwb* ww);

void print_wwb(wwb* ww);

int wwb_to_row_head_string(wwb* wwb,char* out);

int wwb_to_row_string(wwb* wwb,char* out);

#endif /* WWB_H_ */
