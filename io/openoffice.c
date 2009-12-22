/*
 * openoffice.c
 *
 *  Created on: Nov 30, 2009
 *      Author: hannes
 */

#include "openoffice.h"

int formula_required_openoffice_string_length(formula* form){
	junctionIterator it;
	int result;
	switch( form->type ){
		case CONSTANT :
			return 8;
		case VARIABLE :
			if( form->data.variable ){
				result=(int) ceil(log10((double) form->data.variable));
				return result+7;
			}else{
				return 8;
			}
		case NEGATION :
			return formula_required_openoffice_string_length(form->data.negation.inner)+4;
		case JUNCTION :
			result=6;
			it=junction_iterator_get(form);
			while( junction_iterator_valid(it) ){
				result+=formula_required_openoffice_string_length(junction_iterator_formula(it));
				it=junction_iterator_next(it);
			}
			if( form->data.junction.size > 1 ){
				switch( form->data.junction.junctor ){
					case AND :
						result+=5*(form->data.junction.size-1);
						break;
					case OR :
						result+=4*(form->data.junction.size-1);
						break;
					case IMPLICATION :
						result+=8*(form->data.junction.size-1);
						break;
					case EQUIVALENT :
						result+=13*(form->data.junction.size-1);
						break;
					case XOR :
						result+=6*(form->data.junction.size-1);
						break;
				}
			}
			return result;
	}
	return 0;
}
int formula_to_openoffice_string(formula* form,char* out){
	junctionIterator it;
	int offset,subOffset;
	switch( form->type ){
		case CONSTANT :
			if( form->data.constant ){
				sprintf(out," %%verum %n",&offset);
			}else{
				sprintf(out," %%falsum %n",&offset);
			}
			return offset;
		case VARIABLE :
			sprintf(out,"A rsub %d%n",form->data.variable,&offset);
			return offset;
		case NEGATION :
			sprintf(out,"neg %n",&offset);
			offset=formula_to_openoffice_string(form->data.negation.inner,out+offset)+4;
			return offset;
		case JUNCTION :
			sprintf(out," ( %n",&offset);
			it=junction_iterator_get(form);
			if( !junction_iterator_valid(it) ){
				offset++;
			}
			while( junction_iterator_valid(it) ){
				offset+=formula_to_openoffice_string(junction_iterator_formula(it),out+offset);

				it=junction_iterator_next(it);

				if( junction_iterator_valid(it) ){
					switch( form->data.junction.junctor ){
						case AND :
							sprintf(out+offset," and %n",&subOffset);
							break;
						case OR :
							sprintf(out+offset," or %n",&subOffset);
							break;
						case IMPLICATION :
							sprintf(out+offset," toward %n",&subOffset);
							break;
						case EQUIVALENT :
							sprintf(out+offset," %%equivalent %n",&subOffset);
							break;
						case XOR :
							sprintf(out+offset," %%xor %n",&subOffset);
							break;
					}
					offset+=subOffset;
				}
			}
			sprintf(out+offset," ) %n",&subOffset);
			offset+=subOffset;
			return offset;
	}
	return 0;
}

void print_form_openoffice(formula* form){
	char * out=(char *)malloc(formula_required_openoffice_string_length(form));
	formula_to_openoffice_string(form,out);
	printf("%s",out);
}
