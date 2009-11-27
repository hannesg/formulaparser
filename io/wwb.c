/*
 * wwb.c
 *
 *  Created on: Nov 27, 2009
 *      Author: hannes
 */

#include "wwb.h"

int wwb_to_string(wwb* wwb,char* out){
	int offset=0,n;
	if( wwb->left ){
		offset+=wwb_to_string(wwb->left,out);
	}
	sprintf(out+offset,"A%d=%d %n",wwb->variable,wwb->value,&n);
	offset+=n;
	if( wwb->right ){
		offset+=wwb_to_string(wwb->right,out+offset);
	}
	return offset;
}

int wwb_required_string_length(wwb* wwb){
	int length=0;
	if( wwb->left ){
		length+=wwb_required_string_length(wwb->left);
	}
	length+=(int) ceil(log10((double) wwb->variable));
	length+=4;
	if( wwb->right ){
		length+=wwb_required_string_length(wwb->right);
	}
	return length;
}

void print_wwb(wwb* wwb){
	char * out=malloc(sizeof(char)*wwb_required_string_length(wwb));
	wwb_to_string(wwb,out);
	printf("%s",out);
	free(out);
}

int wwb_to_row_head_string(wwb* wwb,char* out){
	int offset=0,n;
	if( wwb->left ){
		offset+=wwb_to_row_head_string(wwb->left,out);
	}
	sprintf(out+offset,"A%-6d%n",wwb->variable,&n);
	offset+=n;
	if( wwb->right ){
		offset+=wwb_to_row_head_string(wwb->right,out+offset);
	}
	return offset;
}

int wwb_to_row_string(wwb* wwb,char* out){
	int offset=0,n;
	if( wwb->left ){
		offset+=wwb_to_row_string(wwb->left,out);
	}
	sprintf(out+offset,"   %d   %n",wwb->value,&n);
	offset+=n;
	if( wwb->right ){
		offset+=wwb_to_row_string(wwb->right,out+offset);
	}
	return offset;
}
