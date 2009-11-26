#include <stdio.h>

#include "formula.h"
#include "io.h"
#include "nf.h"
#include "simplify.h"

int main(int argc,char ** argv){
	formula *current,*old;
	parserResult *result;
	/*=string_to_formula_parser("(-(A0>A1)=A2)");
	formula *unabbr=remove_abbreviations(result->formula);
	formula *nnfo=nnf(unabbr);
	free_formula(result->formula);
	free_formula(unabbr);
	free(result);
	char *out=(char*) malloc(required_string_length(nnfo));
	formula_to_string(nnfo,out);

	printf("%s",out);*/

	if( argc == 1 ){
		printf("Benutzung: %s <formel>\n",argv[0]);
		return 0;
	}else{
		// testen des ersten Arguments
		result=string_to_formula_parser(argv[1]);
		if( result->error || *(result->rest) != 0 ){
			// es ist keine Formel!
			int pos=result->rest-argv[1];
			printf("Nein, falsches Zeichen an Position %d: %s\n",pos,result->rest);
			free(result);
			return 1;
		}
		// es ist eine Formel
		printf("Ja!\n");

		old=result->formula;

		current=remove_abbreviations(old);
		free(old);
		printf("Ohne Abkürzungen: ");
		print_simplified(current);
		printf("\n");
		old=current;

		current=nnf(old);
		printf("NNF: ");
		print_simplified(current);
		printf("\n");
		free_formula(current);

		current=dnf(old);
		printf("DNF: ");
		print_simplified(current);
		printf("\n");
		free_formula(current);

		current=knf(old);
		printf("KNF: ");
		print_simplified(current);
		printf("\n");
		free_formula(current);

		free(result);

		return 0;
	}
}
