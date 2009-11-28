#include <stdio.h>

#include "formula.h"
#include "io.h"
#include "nf.h"
#include "wwb.h"

#include "io/simplify.h"
#include "io/wwb.h"

#include "verifiable/dpll.h"

int main(int argc,char ** argv){
	formula *current,*old;
	parserResult *result;
	wwb* wwb,* oldWwb;
	char *out;
	int verifiable=0,carry=0;
	char *example="(A2+(A1=A3)+-(A0*A4))";

	if( argc == 1 ){
		printf("Benutzung: %s <formel>\n",argv[0]);
		result=string_to_formula_parser(example);
	}else{
		// testen des ersten Arguments
		result=string_to_formula_parser(argv[1]);
	}
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

	wwb=new_wwb();
	verifiable=formula_is_verifiable_dpll(old,wwb);
	if( verifiable ){
		printf("Wahr für:\n");
		print_wwb(wwb);
	}else{
		printf("Widerspruch\n");
	}

	oldWwb=wwb;
	wwb=wwb_from_formula(old);
	wwb_copy_to(oldWwb,wwb);

	out=(char *) malloc(sizeof(char)*100);
	wwb_to_row_head_string(wwb,out);
	printf("\n%s\n",out);
	free(out);

	while( !carry ){
		oldWwb=wwb;
		if( formula_evaluate(old,wwb) ){
			out=(char *) malloc(sizeof(char)*100);
			wwb_to_row_string(wwb,out);
			printf("%s\n",out);
			free(out);
		}
		wwb=wwb_successor(oldWwb,&carry);
		free_wwb(oldWwb);
	}
	free_wwb(wwb);

	free(result);

	return 0;
}
