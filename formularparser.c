#include <stdio.h>

#include "formula.h"
#include "io.h"
#include "nf.h"
#include "simplify.h"

int main(int argc,char ** argv){
	parserResult *result=string_to_formula_parser("(A1*(A2=A3)*(A2|A3))");//
	formula *nnfo=simplify(result->formula);
	free_formula(result->formula);
	free(result);
	char *out=(char*) malloc(required_string_length(nnfo));
	formula_to_string(nnfo,out);

	printf("%s",out);

	if( argc == 1 ){
		printf("Benutzung: %s <formel>\n",argv[0]);
		return 0;
	}else{
		// testen des ersten Arguments
		parserResult *result=string_to_formula_parser(argv[1]);
		if( result->error || *(result->rest) != 0 ){
			// es ist keine Formel!
			int pos=result->rest-argv[1];
			printf("Nein, falsches Zeichen an Position %d: %s\n",pos,result->rest);
			free(result);
			return 1;
		}
		// es ist eine Formel
		printf("Ja!\n");
		return 0;
	}
}
