
#include "dpll.h"

int formula_is_verifiable_dpll_helper(formula * form,wwb* w){
	formula *inner,*formAlt;
	junctionIterator it;
	wwb *wAlt;
	variable var;
	int value=0,subResult;

	it=junction_iterator_get(form);
	while( junction_iterator_valid(it) ){
		inner=junction_iterator_formula(it);
		if( inner->type == JUNCTION  && inner->data.junction.size == 1 ){
			inner=inner->data.junction.part->formula;
		}
		if( inner->type != JUNCTION ){
			// ok, we have a junction with only one part!
			switch( inner->type ){
				case VARIABLE:
					wwb_set_and_lock(w, inner->data.variable,1);
					break ;
				case NEGATION:
					wwb_set_and_lock(w, inner->data.negation.inner->data.variable,0);
					break ;
				default :
					return -1;
					// error!!
			}
			form=formula_evaluate_partially(form,w);
			if( form->type == CONSTANT ){// nice! we are done
				return form->data.constant;
			}
			return formula_is_verifiable_dpll_helper(form,w);
		}
		it=junction_iterator_next(it);
	}
	// still here? looks like we haven't found a junction with only one part
	// so we simply pick the first part of the first junction
	inner=form->data.junction.part->formula->data.junction.part->formula;
	switch( inner->type ){
		case VARIABLE:
			var=inner->data.variable;
			break ;
		case NEGATION:
			var=inner->data.negation.inner->data.variable;
			break ;
		default :
			return -1;
			// error!!
	}
	while( value < 2 ){
		wAlt=clone_wwb(w);
		wwb_set_and_lock(wAlt,var,value);
		formAlt=formula_evaluate_partially(form,wAlt);
		if( formAlt->type == CONSTANT ){// nice! we are done
			free_wwb(wAlt);
			if( formAlt->data.constant ){
				wwb_set_and_lock(w,var,value);
				free_formula(formAlt);
				return 1;
			}else{
				// okay, try next
				free_formula(formAlt);
				value++;
				continue;
			}
		}
		subResult=formula_is_verifiable_dpll_helper(formAlt,wAlt);
		switch( subResult ){
			case 1:
				// possible solution!
				wwb_copy_to(wAlt,w);
				free_wwb(wAlt);
				return 1;
			case 0:
				// next!
				free_wwb(wAlt);
				value++;
				continue;
			default :
				return -1;
		}
		value++;
	}
	return 0;
}

int formula_is_verifiable_dpll(formula * form,wwb* w){
	int result;
	formula* unabbr=remove_abbreviations(form);
	form=knf(unabbr);
	result=formula_is_verifiable_dpll_helper(form,w);
	free_formula(form);
	free_formula(unabbr);
	return result;
}
