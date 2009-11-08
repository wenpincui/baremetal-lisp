#define CASE_DIFFERENCE = 32

/*Packages*/
extern package *keyword_pkg;
extern package *cl_pkg;
extern package *cl_user_pkg;

/*Keywords*/
//Symbol attributes
extern symbol *internal;
extern symbol *external;
extern symbol *inherited;
extern symbol *dynamic;
extern symbol *constant;
//Syntax Types
extern symbol *constituent;
extern symbol *whitespace;
extern symbol *terminating_macro;
extern symbol *non_terminating_macro;
extern symbol *single_escape;
extern symbol *multiple_escape;
extern symbol *alphabetic;
extern symbol *alphadigit;
//Constituent traits
extern symbol *invalid;
extern symbol *package_marker;
extern symbol *plus_sign;
extern symbol *minus_sign;
extern symbol *dot;
extern symbol *decimal_point;
extern symbol *ratio_marker;
//Types
extern symbol *numtype;
extern symbol *realtype;
extern symbol *rattype;//rational
extern symbol *ttype;
extern symbol *listtype;
extern symbol *fixnumtype;
extern symbol *bignumtype;
extern symbol *ratiotype;
extern symbol *singletype;
extern symbol *base_chartype;
extern symbol *vectortype;
extern symbol *arraytype;
extern symbol *compiled_functiontype;
extern symbol *stringtype;
extern symbol *symboltype;
extern symbol *functiontype;
extern symbol *hash_tabletype;
extern symbol *packagetype;
extern symbol *procinfotype;

/*Common-Lisp symbols*/
//Internal symbols, for me.
extern symbol *special_operators;
extern symbol *types;//Internal  list of types.
//Defined symbols
extern symbol *ts;//T symbol
extern symbol *nils;//NIL symbol
extern symbol *package_sym;//*package*
extern symbol *readtable;//*readtable*
//Lambda list control symbols
extern symbol *lambda_list_keywords;
extern symbol *optional;//&optional
extern symbol *rest;//&rest
extern symbol *keyword;//&keyword
extern symbol *aux;//&aux
extern symbol *whole;//&whole
extern symbol *body;//&body
extern symbol *allow_other_keys;//&allow-other-keys
//List function names
extern symbol *cars;//CAR symbol
extern symbol *cdrs;
extern symbol *lists;//LIST symbol
//Special aoperators
extern symbol *quote;//QUOTE symbol
//Equality function names
extern symbol *chareqs;
extern symbol *charequals;
extern symbol *stringeqs;
extern symbol *stringequals;
extern symbol *eqs;
extern symbol *eqls;
//Reader function names
extern symbol *read_chars;
extern symbol *reads;



