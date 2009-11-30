#include "io.h"

parserResult *string_to_formula_parser(char* str){
	char c=*str;
	parserResult *result=(parserResult*) malloc(sizeof(parserResult));
	parserResult *subResult;
	junctor junctor;
	result->error=0;
	int varNum;
	int i=0;
	switch( c ){
		case '(' :// junction
			result->formula=new_junction_formula(0);
			do{
				subResult=string_to_formula_parser(str+1);
				if( *(subResult->rest)==')' ){ // we are done
					add_formula_to_junction(result->formula,subResult->formula);
					result->rest=subResult->rest+1;
					free(subResult);
					return result;
				}
				junctor=get_junctor(*(subResult->rest));
				if( !result->formula->data.junction.junctor ){
					result->formula->data.junction.junctor=junctor;
				}
				if( subResult->error || result->formula->data.junction.junctor != junctor || !is_valid_junctor(junctor) ){
					result->error=1;
					result->rest=subResult->rest;
					free(subResult);
					return result;
				}
				add_formula_to_junction(result->formula,subResult->formula);
				str=subResult->rest;
				free(subResult);
			}while( str );
			free(subResult);
			return result;
		case 'A' :// variable
			if( sscanf(str+1,"%d%n",&varNum,&i) == 0 ){// error
				result->error=1;
				result->rest=str+1;
				return result;
			}
			result->formula=new_variable_formula(varNum);
			result->rest=str+i+1;
			return result;
		case '-' ://negation
			subResult=string_to_formula_parser(str+1);
			result->formula=new_negation_formula(subResult->formula);
			result->rest=subResult->rest;
			result->error=subResult->error;
			free(subResult);
			return result;
		case '0' : // falsum
			result->formula=new_constant_formula(0);
			result->rest=str+1;
			return result;
		case '1' : // verum
			result->formula=new_constant_formula(1);
			result->rest=str+1;
			return result;
	}
	result->error=1;
	result->rest=str;
	return result;
}
int formula_required_string_length(formula* form){
	junctionIterator it;
	int result=1;
	switch( form->type ){
		case CONSTANT :
			return 1;
		case VARIABLE :
			if( form->data.variable ){
				result=(int) ceil(log10((double) form->data.variable));
				return result+1;
			}else{
				return 2;
			}
		case NEGATION :
			return formula_required_string_length(form->data.negation.inner)+1;
		case JUNCTION :
			it=junction_iterator_get(form);
			while( junction_iterator_valid(it) ){
				result+=formula_required_string_length(junction_iterator_formula(it));
				it=junction_iterator_next(it);
			}
			result+=form->data.junction.size+1;
			return result;
	}
	return 0;
}

int formula_to_string(formula* form,char *out){
	junctionIterator it;
	int offset;
	switch( form->type ){
		case CONSTANT :
			if( form->data.constant ){
				out[0]='1';
			}else{
				out[0]='0';
			}
			out[1]='\0';
			return 1;
		case VARIABLE :
			sprintf(out,"A%d%n",form->data.variable,&offset);
			out[offset]='\0';
			return offset;
		case NEGATION :
			out[0]='-';
			offset=formula_to_string(form->data.negation.inner,out+1)+1;
			out[offset]='\0';
			return offset;
		case JUNCTION :
			out[0]='(';
			offset=0;
			it=junction_iterator_get(form);
			if( !junction_iterator_valid(it) ){
				offset++;
			}
			while( junction_iterator_valid(it) ){
				offset++;
				offset+=formula_to_string(junction_iterator_formula(it),out+offset);
				out[offset]=form->data.junction.junctor;
				it=junction_iterator_next(it);
			}
			out[offset]=')';
			offset++;
			out[offset]='\0';
			return offset;
	}
	return 0;
}
void print_form(formula* form){
	char * out=(char *)malloc(formula_required_string_length(form));
	formula_to_string(form,out);
	printf("%s",out);
}
