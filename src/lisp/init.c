/* init.c
 *
 * This file contains all code to set up a working Lisp environment.
 *
 * This code is released under the GNU GPL General Public License.
 */

#include <stdlib.h>
#include "lisp.h"
#include "init.h"
#include "lbind.h"

cons *basic_classes[20];

/* For initializaton, these don't need to be flexible. */
procinfo *proc;
cons *basic_env;

/* Packages */
package *keyword_pkg;
package *cl_pkg;
package *cl_user_pkg;

/* Keywords */
/* Symbol attributes */
symbol *internal;
symbol *external;
symbol *inherited;
symbol *special;
symbol *constant;
/* Function Attributes */
symbol *special_operator;
/* Syntax Types */ 
symbol *constituent;
symbol *whitespace;
symbol *terminating_macro;
symbol *non_terminating_macro;
symbol *single_escape;
symbol *multiple_escape;
symbol *alphabetic;
symbol *alphadigit;
/* Constituent traits */
symbol *invalid;
symbol *package_marker;
symbol *plus_sign;
symbol *minus_sign;
symbol *dot;
symbol *decimal_point;
symbol *ratio_marker;

/* Common-Lisp symbols */
/* Types */
symbol *built_in_class_s;
symbol *number_s;
symbol *real_s;
symbol *rational_s;
symbol *integer_s;
symbol *fixnum_s;
symbol *bignum_s;
symbol *ratio_s;
symbol *complex_s;
symbol *float_s;
symbol *single_s;
symbol *character_s;
symbol *base_char_s;
symbol *extended_char_s;
symbol *sequence_s;
symbol *cons_s;
symbol *null_s;
symbol *vector_s;
symbol *array_s;
symbol *compiled_function_s;
symbol *string_s;
symbol *symbol_s;
symbol *function_s;
symbol *hash_table_s;
symbol *package_s;
symbol *procinfo_s;
symbol *class_s;
/* Defined symbols */
symbol *t_s;/* T symbol */
symbol *nil_s;/* NIL symbol */
symbol *package_s;/*  *package*  */
symbol *readtable_s;/*  *readtable*  */
/* Lambda list control symbols */
symbol *lambda_list_keywords_s;
symbol *optional_s;/* &optional */
symbol *rest_s;/* &rest */
symbol *keyword_s;/* &keyword */
symbol *aux_s;/* &aux */
symbol *whole_s;/* &whole */
symbol *body_s;/* &body */
symbol *allow_other_keys_s;/* &allow-other-keys */
/* List function names */
symbol *car_s;/* CAR symbol */
symbol *cdr_s;/* CDR symbol */
symbol *list_s;/* LIST symbol */
/* Special operators */
symbol *quote_s;/* QUOTE symbol */
/* Assignment Operator names */
symbol *defun_s;
/* Equality function names */
symbol *chareq_s;
symbol *charequal_s;
symbol *stringeq_s;
symbol *stringequal_s;
symbol *eq_s;
symbol *eql_s;
symbol *equal_s;
symbol *equalp_s;
/* Reader function names */
symbol *read_char_s;
symbol *read_s;

/*Local functions*/
symbol *initsym(char *name, package *p);
symbol *initintsym(char *name, package *p);
symbol *initcfun (char *name, 
		  cons *lambda_list, 
		  package *p, 
		  cons *(*fun)(cons *env));
cons *initdeftype(char *string);

void init_keyword_pkg();

void init_cl_pkg();
void init_readtable();
void init_lambda_control();
void init_special_operators();
void init_list_funs();
void init_eq_funs();
void init_read_funs();
void init_set_funs();
void init_types();

cons *initread(stream *str, cons *env);


/*Initialization helper functions*/
symbol *initsym(char *name, package *p)
{/* Initialize an external symbol named name in package p.
  */
  array *a_name = strtolstr(name);
  symbol *a = intern(a_name, p);
  if (p == keyword_pkg)
    {/* Keywords suffer from apartheid, as per CLHS: 
      */
      a->plist = fcons(fcons((cons*)external, t), 
		       fcons(fcons((cons*)constant, t), nil));
      a->value = (cons*)a;
    }
  else
      a->plist = fcons(fcons((cons*)external, t), nil);
  return a;
}

symbol *initintsym(char *name, package *p)
{/*Initialize an internal symbol named name in package p.
  */
  array *a_name = strtolstr(name);
  symbol *a = intern(a_name, p);
  
  a->plist = fcons(fcons((cons*)internal, t), nil);
  return a;
}

symbol *initcfun (char *name, 
		  cons *lambda_list, 
		  package *p, 
		  cons *(*fun)(cons *env))
{/* Initialize a compiled C function in package p. 
  */
  symbol *funsym;
  compiled_function *f;

  funsym = intern(strtolstr(name), p);
  funsym->plist = fcons(fcons((cons*)external, t), 
			fcons(fcons((cons*)constant, t), nil));
  funsym->fun = (function*)newcompiled_function();
  f = (compiled_function*)funsym->fun;
  f->plist = fcons(fcons((cons*)external, t), 
		   fcons(fcons((cons*)constant, t), nil));
  f->env = basic_env;
  f->lambda_list = lambda_list;
  f->fun = fun;
  return funsym;
}

cons *initdeftype(char *string)
{
  stream *str = newstream();
  str->rv = strtolstr(string);
  cons *foo = initread(str, basic_env);
  foo->type = (cons*)BUILT_IN_CLASS;
  return foo;
}

cons *initread(stream *str, cons *env)
{/* An inflexible, incomplete implementation of read. Just what the programmer
  * needs to get the system off the ground, and not an iota more. To be extended
  * at the programmer's whim for convenience. Currently recognizes: lists, 
  * characters, symbols
  *
  * Stream support is rudimentary.
  */
  package *p = (package*)((procinfo*)env->car)->package_s->value;
  base_char *c = read_char(str);

  while (1)
    {
      if (c->c == 0 || c->c == ')')
	{/* Terminate a list, or a read.
	  */
	  if (c->c == 0)
	    /* If the character is the null character, unread it, or else
	     * successive reads won't know that the stream has ended.
	     */
	    unread_char(c, str);
	  return nil;
	}
      else if (c->c == ' ' || c->c == '\t')
	/* Skip over whitespace.
	 */
	c = read_char(str);
      else if (c->c == '(')
	{/* Read a list.
	  */
	  cons *foo = newcons();
	  cons *bar;
	  cons *to_ret = foo;

	  bar = (cons*)initread(str, env);
	  if (bar == nil)
	    /*Recognize when the list is null.
	     */
	    return nil;
	  else
	    /* Otherwise, start reading a list!
	     */
	    foo->car = bar;
	  
	  while ((c->c != ')') &&
		 (c->c != 0))
	    {/* Read a list, until a read returns nil, terminating the list.
	      */
	      bar = (cons*)initread(str, env);
	      if (bar == nil)
		return to_ret;
	      foo->cdr = newcons();
	      foo = foo->cdr;
	      foo->car = bar;
	    }
	}
      else if (c->c >='0' && c->c <='9')
	/* If the token begins with a digit, parse a number.
	 */
	return 0;//TODO 
      else if (c->c == '#')
	{/* If the character is the macro sharpsign, interpret it.
	  */
	  c = read_char(str);
	  if (c->c == '\\')
	    {/* If #\ is the reader macro combination, read a character.
	      */
	      c = read_char(str);
	      return (cons*)ctolc(c->c);
	    }
	  else
	    c = read_char(str);
	}
      else
	{/* Otherwise, we are reading a symbol.
	  */
	  int i=0;
	  char name[100];
	  cons *foo=0;

	  while ((c->c != '(') &&
		 (c->c != ')') &&
		 (c->c != ' ') &&
		 (c->c != 0))
	    {/* Until we encounter something to terminate the symbol, read 
	      * characters into a buffer. Programmer: No symbols of more than 99
	      * characters if  you're using this function, and are too lazy to 
	      * modify it. (Safe to change, just an arbitrary buffer size.)
	      */
	      if (i >= 100)
		/* Even with the warning above, check to see if the programmer
		 * ignored my warnings. Prevents buffer overruns.
		 */
		return 0;//error

	      if ((c->c >= 'a') &&
		  (c->c <='z'))
		/* Convert the character to uppercase.
		 */
		c->c = c->c - 'A';

	      name[i] = c->c;
	      c = read_char(str);
	      i++;
	    }
	  if (i >= 100)
	    /* If we overrun the buffer, we are in error. return.
	     */
	    return 0;
	  else
	    name[i] = 0;
	  
	  if (c->c == 0)
	    /* If the character is the null character, unread it, or else
	     * successive reads won't know that the stream has ended.
	     */
	    unread_char(c, str);

	  if (name[0] == ':')
	    p = keyword_pkg;
	  
	  return (cons*)intern(strtolstr(name), p);
	}
    }
}

/*Initialization*/
void init_keyword_pkg()
{/* Initialize keywords that we'll need to get things rolling.
  */
  array *keyword_name = strtolstr("KEYWORD");
  keyword_pkg = newpackage();
  keyword_pkg->name = keyword_name;

  /* External and constant must be initialized manually, because they depend on
   * themselves.
   */
  external = intern(strtolstr("EXTERNAL"), keyword_pkg);
  external->value = (cons*)external;
  constant = intern(strtolstr("CONSTANT"), keyword_pkg);
  constant->value = (cons*)constant;
  external->plist = fcons(fcons((cons*)external, t), 
			  fcons(fcons((cons*)constant, t), nil));
  constant->plist = fcons(fcons((cons*)external, t), 
			  fcons(fcons((cons*)constant, t), nil));

  /* Other symbol attributes*/
  internal = initsym("INTERNAL", keyword_pkg);
  inherited = initsym("INHERITED", keyword_pkg);
  special = initsym("SPECIAL", keyword_pkg);

  special_operator = initsym("SPECIAL-OPERATOR", keyword_pkg);

  /* Readtable character attributes */
  constituent = initsym("CONSTITUENT", keyword_pkg);
  whitespace = initsym("WHITESPACE", keyword_pkg);
  terminating_macro = initsym("TERMINATING-MACRO-CHARACTER", keyword_pkg);
  non_terminating_macro = initsym("NON-TERMINATING-MACRO-CHARACTER", 
				  keyword_pkg);
  single_escape = initsym("SINGLE-ESCAPE", keyword_pkg);
  multiple_escape = initsym("MULTIPLE-ESCAPE", keyword_pkg);

  /* Consituent traits */
  invalid = initsym("INVALID", keyword_pkg);
  alphabetic = initsym("ALPHABETIC", keyword_pkg);
  alphadigit = initsym("ALPHADIGIT", keyword_pkg);
  package_marker = initsym("PACKAGE-MARKER", keyword_pkg);
}

void init_cl_pkg()
{/* Initialize the Common Lisp package, and its contents.
  */
  array *cl_name = strtolstr("COMMON-LISP");
  cl_pkg = newpackage();
  cl_pkg->name = cl_name;

  /* Init T */
  t->type = (cons*)T;
  t_s = initsym("T", cl_pkg);
  t_s->value = t;
  
  /* Init NIL */
  nil->type = (cons*)LIST;
  nil->car = nil;
  nil->cdr = nil;
  nil_s = initsym("NIL", cl_pkg);

  nil_s->value = nil;

  package_s = initsym("*PACKAGE*", cl_pkg);
  package_s->value = (cons*)cl_pkg;
  proc->package_s  = package_s;
  init_types();
  init_lambda_control();
  init_list_funs();
  init_eq_funs();
  init_read_funs();
  init_set_funs();
}

void init_lambda_control()
{/* Initialize the lambda control characters for evalambda().
  */
  optional_s = initsym("&OPTIONAL", cl_pkg);
  rest_s = initsym("&REST", cl_pkg);
  keyword_s = initsym("&KEYWORD", cl_pkg);
  aux_s = initsym("&AUX", cl_pkg);
  whole_s = initsym("&WHOLE", cl_pkg);
  body_s = initsym("&BODY", cl_pkg);
  allow_other_keys_s = initsym("&ALLOW-OTHER-KEYS", cl_pkg);

  lambda_list_keywords_s = initsym("LAMBDA-LIST-KEYWORDS", cl_pkg);
  lambda_list_keywords_s->value = fcons((cons*)optional_s,
				      fcons((cons*)rest_s,
					    fcons((cons*)keyword_s,
						  fcons((cons*)aux_s,
							fcons((cons*)whole_s,
							      fcons((cons*)body_s,
								    fcons((cons*)allow_other_keys_s, nil)))))));
}

void init_list_funs()
{/* Initialize the list functions.
  */
  car_s = initcfun("CAR", 
		  fcons((cons*)intern(strtolstr("LIST"), cl_pkg),
			nil),
		  cl_pkg,
		  &lcar);
  cdr_s = initcfun("CDR", 
		  fcons((cons*)intern(strtolstr("LIST"), cl_pkg),
			nil),
		  cl_pkg,
		  &lcar);
  quote_s = initcfun("QUOTE", 
		   fcons((cons*)intern(strtolstr("EXP"), cl_pkg),
			 nil),
		   cl_pkg,
		   &lquote);  
  list_s = initcfun("LIST",
		   fcons((cons*)rest_s, 
			 fcons((cons*)intern(strtolstr("ARGS"), cl_pkg),
			       nil)),
		   cl_pkg,
		   &llist);  
}

void init_eq_funs()
{/* Initialize the equality functions.
  */
  chareq_s = initcfun("CHAR=",
		     fcons((cons*)intern(strtolstr("A"), cl_pkg),
			   fcons((cons*)intern(strtolstr("B"), cl_pkg),
				 nil)),
		     cl_pkg,
		     &lchareq);
  charequal_s = initcfun("CHAR-EQUAL",
			fcons((cons*)intern(strtolstr("A"), cl_pkg),
			      fcons((cons*)intern(strtolstr("B"), cl_pkg),
				    nil)),
			cl_pkg,
			&lcharequal);

  stringeq_s = initcfun("STRING=",
		       fcons((cons*)intern(strtolstr("A"), cl_pkg),
			     fcons((cons*)intern(strtolstr("B"), cl_pkg),
				   nil)),
		       cl_pkg,
		       &lstringeq);
  
  stringequal_s = initcfun("STRING-EQUAL",
			  fcons((cons*)intern(strtolstr("A"), cl_pkg),
				fcons((cons*)intern(strtolstr("B"), cl_pkg),
				      nil)),
			  cl_pkg,
			  &lstringequal);
  
  eq_s = initcfun("EQ", 
		 fcons((cons*)intern(strtolstr("A"), cl_pkg), 
		       fcons((cons*)intern(strtolstr("B"), cl_pkg), 
			     nil)), 
		 cl_pkg, 
		 &leq);

  eql_s = initcfun("EQL",
		  fcons((cons*)intern(strtolstr("A"), cl_pkg),
			fcons((cons*)intern(strtolstr("B"), cl_pkg),
			      nil)),
		  cl_pkg,
		  &leql);
}

void init_read_funs()
{/* Initialize reader functions.
  */
  stream *str = newstream();
  str->rv = strtolstr("(&OPTIONAL (STREAM *STANDARD-INPUT*) (EOF-ERROR-P T))");
  str->write_index = str->rv->length->num;

  read_char_s = initcfun("READ-CHAR",
			initread(str, basic_env),
			cl_pkg,
			&lread_char);
}  

void init_set_funs()
{/* Initialize assignment functions.
  */
  defun_s = initcfun("DEFUN",
		  fcons((cons*)intern(strtolstr("SYMBOL"), cl_pkg),
			fcons((cons*)intern(strtolstr("LAMBDA-LIST"), cl_pkg),
			      fcons((cons*)intern(strtolstr("FORM"), cl_pkg),
				    nil))),
		  cl_pkg,
		  &leql);
}

void init_types()
{/* Initialize startup types.
  */

  t_s->class = initdeftype("(T () ())");
  basic_classes[T] = t_s->class;
  class_s = initsym("CLASS", cl_pkg);
  class_s->class = initdeftype("(CLASS (T) ())");
  built_in_class_s = initsym("BUILT-IN-CLASS", cl_pkg);
  built_in_class_s->class = initdeftype("(BUILT-IN-CLASS (CLASS) ())");

  number_s = initsym("NUMBER", cl_pkg);
  number_s->class = initdeftype("(NUMBER (BUILT-IN-CLASS) ())");
  real_s = initsym("REAL", cl_pkg);
  real_s->class = initdeftype("(REAL (NUMBER) ())");

  rational_s = initsym("RATIONAL", cl_pkg);
  rational_s->class = initdeftype("(RATIONAL (REAL) ())");
  integer_s = initsym("INTEGER", cl_pkg);
  integer_s->class = initdeftype("(INTEGER (RATIONAL) ())");
  fixnum_s = initsym("FIXNUM", cl_pkg);
  fixnum_s->class = initdeftype("(FIXNUM (INTEGER) ())");
  basic_classes[FIXNUM] = fixnum_s->class;
  bignum_s = initsym("BIGNUM", cl_pkg);
  bignum_s->class = initdeftype("(BIGNUM (INTEGER) ())");
  basic_classes[BIGNUM] = bignum_s->class;
  ratio_s = initsym("RATIO", cl_pkg);
  ratio_s->class = initdeftype("(RATIO (RATIONAL) ())");
  basic_classes[RATIO] = ratio_s->class;

  float_s = initsym("FLOAT", cl_pkg);
  float_s->class = initdeftype("(FLOAT (REAL) ())");
  single_s = initsym("SINGLE-FLOAT", cl_pkg);
  single_s->class = initdeftype("(SINGLE-FLOAT (FLOAT) ())");
  basic_classes[SINGLE] = single_s->class;

  complex_s = initsym("COMPLEX", cl_pkg);
  complex_s->class = initdeftype("(COMPLEX (NUMBER) ())");
  
  sequence_s = initsym("SEQUENCE", cl_pkg);
  sequence_s->class = initdeftype("(SEQUENCE (T) ())");
  
  list_s = initsym("LIST", cl_pkg);
  list_s->class = initdeftype("(LIST (SEQUENCE) ())");
  basic_classes[LIST] = list_s->class;
  cons_s = initsym("CONS", cl_pkg);
  cons_s->class = initdeftype("(CONS (LIST) ())");
  null_s = initsym("NULL", cl_pkg);
  null_s->class = initdeftype("(NULL (SYMBOL LIST) ())");

  array_s = initsym("ARRAY", cl_pkg);
  array_s->class = initdeftype("(ARRAY (T) ())");
  basic_classes[ARRAY] = array_s->class;
  vector_s = initsym("VECTOR", cl_pkg);
  vector_s->class = initdeftype("(VECTOR (ARRAY SEQUENCE) ())");
  string_s = initsym("STRING", cl_pkg);
  string_s->class = initdeftype("(STRING (VECTOR) ())");
  basic_classes[STRING] = string_s->class;

  character_s = initsym("CHARACTER", cl_pkg);
  character_s->class = initdeftype("(CHARACTER (T) ())");
  base_char_s = initsym("BASE-CHARACTER", cl_pkg);
  base_char_s->class = initdeftype("(BASE-CHARACTER (CHARACTER) ())");
  basic_classes[BASE_CHAR] = base_char_s->class;
  extended_char_s = initsym("EXTENDED-CHARACTER", cl_pkg);
  extended_char_s->class = initdeftype("(EXTENDED-CHARACTER (CHARACTER) ())");

  function_s = initsym("FUNCTION", cl_pkg);
  function_s->class = initdeftype("(FUNCTION (T) ())");
  basic_classes[FUNCTION] = function_s->class;
  compiled_function_s = initsym("COMPILED-FUNCTION", cl_pkg);
  compiled_function_s->class = initdeftype("(COMPILED-FUNCTION (FUNCTION) ())");
  basic_classes[COMPILED_FUNCTION] = compiled_function_s->class;

  symbol_s = initsym("SYMBOL", cl_pkg);
  symbol_s->class = initdeftype("(SYMBOL (T) ())");
  basic_classes[SYMBOL] = symbol_s->class;
  
  package_s = initsym("PACKAGE", cl_pkg);
  package_s->class = initdeftype("(PACKAGE (T) ())");
  basic_classes[PACKAGE] = package_s->class;

  procinfo_s = initintsym("PROCESS-INFO", cl_pkg);
  procinfo_s->class = initdeftype("(PROCESS-INFO (T) ())");
  basic_classes[PROCINFO] = procinfo_s->class;

  nil->type = null_s->class;
}

procinfo *init()
{
  long i;
  long j;

  basic_env = extend_env(nil);

  /* Init process info */
  proc = malloc(sizeof(procinfo));
  proc->type = (cons*)PROCINFO;
  proc->packages = newcons();
  proc->packages->car = (cons*)cl_pkg;
  proc->packages->cdr = newcons();
  proc->packages->cdr->car = (cons*)cl_user_pkg;
  proc->packages->cdr->cdr = newcons();
  proc->packages->cdr->cdr->car = (cons*)keyword_pkg;  
  basic_env->car = (cons*)proc;

  init_keyword_pkg();
  init_cl_pkg();
  
  /* Init cl-user package */
  array *cl_user_name = strtolstr("COMMON_LISP_USER");
  cl_user_pkg = newpackage();
  cl_user_pkg->name = cl_user_name;


  return proc;
}

