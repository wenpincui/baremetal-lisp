#include <stdlib.h>
#include "lisp.h"
#include "init.h"
#include "lbind.h"

/*For initializaton, these don't need to be flexible.*/
procinfo *proc;
cons *basic_env;

/*Packages*/
package *keyword_pkg;
package *cl_pkg;
package *cl_user_pkg;

/*Keywords*/
//Symbol attributes
symbol *internal;
symbol *external;
symbol *inherited;
symbol *dynamic;
symbol *constant;
//Syntax Types
symbol *constituent;
symbol *whitespace;
symbol *terminating_macro;
symbol *non_terminating_macro;
symbol *single_escape;
symbol *multiple_escape;
symbol *alphabetic;
symbol *alphadigit;
//Constituent traits
symbol *invalid;
symbol *package_marker;
symbol *plus_sign;
symbol *minus_sign;
symbol *dot;
symbol *decimal_point;
symbol *ratio_marker;
//Types
symbol *numtype;
symbol *realtype;
symbol *rattype;//rational
symbol *ttype;
symbol *constype;
symbol *fixnumtype;
symbol *bignumtype;
symbol *ratiotype;
symbol *singletype;
symbol *base_chartype;
symbol *vectortype;
symbol *arraytype;
symbol *compiled_functiontype;
symbol *stringtype;
symbol *symboltype;
symbol *functiontype;
symbol *hash_tabletype;
symbol *packagetype;
symbol *procinfotype;

/*Common-Lisp symbols*/
symbol *ts;//T symbol
symbol *nils;//NIL symbol
symbol *package_sym;//*package*
symbol *readtable;//*readtable*
symbol *types;//Internal  list of types.
//Lambda list control symbols
symbol *lambda_list_keywords;
symbol *optional;//&optional
symbol *rest;//&rest
symbol *keyword;//&keyword
symbol *aux;//&aux
symbol *whole;//&whole
symbol *body;//&body
symbol *allow_other_keys;//&allow-other-keys
//List function anmes
symbol *cars;//CAR symbol
symbol *quote;//QUOTE symbol
//Equality function names
symbol *eqs;
symbol *eqls;

/*Local functions*/
symbol *initsym(char *name, package *p);
symbol *initcfun (char *name, cons *lambda_list, package *p, cons *(*fun)(cons *env));

void init_keyword_pkg();

void init_cl_pkg();
void init_readtable();
void init_lambda_control();
void init_list_funs();
void init_eq_funs();


//This adorable little function saved me so much manual work. Global variables, here, are acceptable. :]
symbol *initsym(char *name, package *p)
{
  simple_vector *a_name = strtolstr(name);
  symbol *a = intern(a_name, p);
  if (p == keyword_pkg)
    {
      a->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
      a->value = (cons*)a;
    }
  else if (p == cl_pkg)
    {
      a->plist = fcons(fcons((cons*)external, t), nil);
    }
  return a;
}

symbol *initcfun (char *name, cons *lambda_list, package *p, cons *(*fun)(cons *env))
{
  symbol *funsym = intern(strtolstr(name), p);
  funsym->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
  funsym->fun = (function*)newcompiled_function();
  compiled_function *f = (compiled_function*)funsym->fun;
  f->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
  f->env = basic_env;
  f->lambda_list = lambda_list;
  f->fun = fun;
  return funsym;
}

void init_keyword_pkg()
{
  //Init keyword package
  simple_vector *keyword_name = strtolstr("KEYWORD");
  keyword_pkg = newpackage();
  keyword_pkg->name = keyword_name;

  //External and constant must be initialized manually, because they depend on themselves.
  external = intern(strtolstr("EXTERNAL"), keyword_pkg);
  external->value = (cons*)external;
  constant = intern(strtolstr("CONSTANT"), keyword_pkg);
  constant->value = (cons*)constant;
  external->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));
  constant->plist = fcons(fcons((cons*)external, t), fcons(fcons((cons*)constant, t), nil));

  //Other symbol attributes
  internal = initsym("INTERNAL", keyword_pkg);
  inherited = initsym("INHERITED", keyword_pkg);
  dynamic = initsym("DYNAMIC", keyword_pkg);

  //Readtable character attributes
  constituent = initsym("CONSTITUENT", keyword_pkg);
  whitespace = initsym("WHITESPACE", keyword_pkg);
  terminating_macro = initsym("TERMINATING-MACRO-CHARACTER", keyword_pkg);
  non_terminating_macro = initsym("NON-TERMINATING-MACRO-CHARACTER", keyword_pkg);
  single_escape = initsym("SINGLE-ESCAPE", keyword_pkg);
  multiple_escape = initsym("MULTIPLE-ESCAPE", keyword_pkg);

  //Consituent traits
  invalid = initsym("INVALID", keyword_pkg);
  alphabetic = initsym("ALPHABETIC", keyword_pkg);
  alphadigit = initsym("ALPHADIGIT", keyword_pkg);
  package_marker = initsym("PACKAGE-MARKER", keyword_pkg);

  //Types
  ttype = initsym("T", keyword_pkg);
  constype = initsym("CONS", keyword_pkg);
  fixnumtype = initsym("FIXNUM", keyword_pkg);
  bignumtype = initsym("BIGNUM", keyword_pkg);
  ratiotype = initsym("RATIO", keyword_pkg);
  singletype = initsym("SINGLE", keyword_pkg);
  base_chartype = initsym("BASE-CHARACTER", keyword_pkg);
  vectortype = initsym("SIMPLE_VECTOR", keyword_pkg);
  arraytype = initsym("ARRAY", keyword_pkg);
  compiled_functiontype = initsym("COMPILED-FUNCTION", keyword_pkg);
  stringtype = initsym("STRING", keyword_pkg);
  symboltype = initsym("SYMBOL", keyword_pkg);
  functiontype = initsym("FUNCTION", keyword_pkg);
  hash_tabletype = initsym("HASH-TABLE", keyword_pkg);
  packagetype = initsym("PACKAGE", keyword_pkg);
  procinfotype = initsym("PROCESS-INFO-TABLE", keyword_pkg);
}

void init_cl_pkg()
{
  //Init cl package
  simple_vector *cl_name = strtolstr("COMMON-LISP");
  cl_pkg = newpackage();
  cl_pkg->name = cl_name;

  //Init t
  t->type = T;
  ts = initsym("T", cl_pkg);
  ts->value = t;
  
  //Init nil
  //simple_vector *nil_name = strtolstr("NIL");
  nil->type = CONS;
  nil->car = nil;
  nil->cdr = nil;
  nils = initsym("NIL", cl_pkg);

  nils->value = nil;

  package_sym = initsym("*PACKAGE*", cl_pkg);
  package_sym->value = (cons*)cl_pkg;
  init_readtable();
  init_lambda_control();
  init_list_funs();
  init_eq_funs();
}

void init_readtable()
{
  int i;

  //Init *readtable*
  simple_vector *readtable_name = strtolstr("*READTABLE*");
  readtable = intern(readtable_name, cl_pkg);
  readtable->value = (cons*)newsimple_vector(255);
  char alphabetic_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  char alphadigit_chars[] = "0123456789";
  char whitespace_chars[] = " \t\n";
  char terminating_macro_chars[] = "\"\'(),;`";
  char non_terminating_macro_chars[] = "#";
  char single_escape_chars[] = "\\";
  char multiple_escape_chars[] = "|";
  char constituents[] = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ1234567890!$_-./:;?+<=>#%&*@[]{}^~";

  char *c;
  base_char *bc;

  //Create the default readtable
  //Use the numerical ASCII values of each number as the index of a readtable, a simple_vector.
  //Each entry has an associated property list, as per CLHS 2.1.4.

  for (i=0;i<' ';i++)
    ((simple_vector*)readtable->value)->v[i] = fcons(fcons((cons*)invalid, t), ((simple_vector*)readtable->value)->v[i]);

  for (c=constituents;*c!=0;c++)
    ((simple_vector*)readtable->value)->v[*c] = fcons(fcons((cons*)constituent, t), ((simple_vector*)readtable->value)->v[*c]);
  //init constituents
  ((simple_vector*)readtable->value)->v[':'] =  fcons(fcons((cons*)package_marker, t), ((simple_vector*)readtable->value)->v[':']);

  /*Init specific attributes*/
  for (c=alphabetic_chars;*c!=0;c++)
    ((simple_vector*)readtable->value)->v[*c] = fcons(fcons((cons*)alphabetic, t), ((simple_vector*)readtable->value)->v[*c]);	       

  for (c=alphadigit_chars;*c!=0;c++)
    ((simple_vector*)readtable->value)->v[*c] = fcons(fcons((cons*)alphadigit, t), ((simple_vector*)readtable->value)->v[*c]);

  for (c=terminating_macro_chars;*c!=0;c++)
    ((simple_vector*)readtable->value)->v[*c] =  fcons(fcons((cons*)terminating_macro, nil), ((simple_vector*)readtable->value)->v[*c]);

  for (c=whitespace_chars;*c!=0;c++)
    ((simple_vector*)readtable->value)->v[*c] = fcons(fcons((cons*)whitespace, t), fcons((cons*)invalid, ((simple_vector*)readtable->value)->v[*c]));

  for (c=single_escape_chars;*c!=0;c++)
    ((simple_vector*)readtable->value)->v[*c] =  fcons(fcons((cons*)single_escape, t), ((simple_vector*)readtable->value)->v[*c]); 

  for (c=multiple_escape_chars;*c!=0;c++)
    ((simple_vector*)readtable->value)->v[*c] =  fcons(fcons((cons*)multiple_escape, t), ((simple_vector*)readtable->value)->v[*c]);

  for(i=0;i<((simple_vector*)readtable->value)->size;i++)
    if (((simple_vector*)readtable->value)->v[i] == nil)
      ((simple_vector*)readtable->value)->v[*c] =  fcons(fcons((cons*)invalid, t), ((simple_vector*)readtable->value)->v[*c]);
  //Invalidate the rest, the wretched lot...  
}

void init_lambda_control()
{
  //Init lambda list control symbols
  optional = initsym("&OPTIONAL", cl_pkg);
  rest = initsym("&REST", cl_pkg);
  keyword = initsym("&KEYWORD", cl_pkg);
  aux = initsym("&AUX", cl_pkg);
  whole = initsym("&WHOLE", cl_pkg);
  body = initsym("&BODY", cl_pkg);
  allow_other_keys = initsym("&ALLOW-OTHER-KEYS", cl_pkg);

  lambda_list_keywords = initsym("LAMBDA-LIST-KEYWORDS", cl_pkg);
  lambda_list_keywords->value = fcons((cons*)optional,
				      fcons((cons*)rest,
					    fcons((cons*)keyword,
						  fcons((cons*)aux,
							fcons((cons*)whole,
							      fcons((cons*)body,
								    fcons((cons*)allow_other_keys, nil)))))));
}

void init_list_funs()
{
  //symbol *initcfun (char *name, cons *lambda_list, package *p, cons *(*fun)(cons *env));
  cars = initcfun("CAR", 
		 fcons((cons*)intern(strtolstr("LIST"), cl_pkg),
		       nil),
		 cl_pkg,
		 &lcar);
  quote = initcfun("QUOTE", 
		   fcons((cons*)intern(strtolstr("EXP"), cl_pkg),
			 nil),
		   cl_pkg,
		   &lquote);  
}  

void init_eq_funs()
{
  eqs = initcfun("EQ",
		fcons((cons*)intern(strtolstr("A"), cl_pkg),
		      fcons((cons*)intern(strtolstr("B"), cl_pkg),
			    nil)),
		cl_pkg,
		&leq);
  eqls = initcfun("EQL",
		 fcons((cons*)intern(strtolstr("A"), cl_pkg),
		       fcons((cons*)intern(strtolstr("B"), cl_pkg),
			     nil)),
		 cl_pkg,
		 &leql);
}
  

procinfo *init()
{
  long i;
  long j;

  basic_env = extend_env(nil);
  proc = malloc(sizeof(procinfo));
  basic_env->car = (cons*)proc;

  init_keyword_pkg();
  init_cl_pkg();
  
  //Init cl-user package
  simple_vector *cl_user_name = strtolstr("COMMON_LISP_USER");
  cl_user_pkg = newpackage();
  cl_user_pkg->name = cl_user_name;

  //init process info
  proc->type = PROCINFO;
  proc->package_sym  = package_sym;
  proc->packages = newcons();
  proc->packages->car = (cons*)cl_pkg;
  proc->packages->cdr = newcons();
  proc->packages->cdr->car = (cons*)cl_user_pkg;
  proc->packages->cdr->cdr = newcons();
  proc->packages->cdr->cdr->car = (cons*)keyword_pkg;  

  return proc;
}

