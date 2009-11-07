#define HASH_TABLE_SIZE 255

//Types
#define T 1
#define LIST 2
#define FIXNUM 3
#define BIGNUM 4
#define RATIO 5
#define SINGLE 6
#define BASE_CHAR 7
#define SIMPLE_VECTOR 8
#define ARRAY 9
#define COMPILED_FUNCTION 10
#define STRING 11
#define SYMBOL 12
#define FUNCTION 13
#define HASH_TABLE 14
#define PACKAGE 15
#define PROCINFO 16

#define BITS32

#ifdef BITS32

typedef struct cons
{
  unsigned short type;
  struct cons *car;
  struct cons *cdr;
} __attribute__((packed)) cons;

//Numbers
//Integers
typedef struct fixnum
{
  unsigned short type;
  long num;
} __attribute__((packed)) fixnum;

typedef struct bignum
{
  unsigned short type;
  long num;
  struct bignum *next;
} __attribute__((packed)) bignum;

//Ratios
typedef struct ratio
{
  unsigned short type;
  struct fixnum *numerator;
  struct fixnum *denominator;
} __attribute__((packed)) ratio;

//Floating point
typedef struct single
{
  unsigned short type;
  unsigned short sign : 1;
  unsigned short base : 15;
  struct fixnum *exponent;
  struct fixnum *integer;
} __attribute__((packed)) single;

//Characters
//Standard character
typedef struct base_char
{
  unsigned short type;
  char c;
}__attribute__((packed)) base_char;

typedef struct array
{
  unsigned short type;
  struct cons *plist;
  struct fixnum *rank;
  struct fixnum *length;
  struct cons ***a;//Array
  struct array *next;//Just in case we want to expand it.
}__attribute__((packed)) array;

//Functions
typedef struct compiled_function
{
  unsigned short type;
  struct cons *plist;
  struct cons *lambda_list;
  struct cons *env;
  struct cons *(*fun)(cons* env);
}__attribute__((packed)) compiled_function;

typedef struct function
{
  unsigned short type;
  struct cons *plist;
  struct cons *lambda_list;
  struct cons *env;
  struct cons *fun;
}__attribute__((packed)) function;

typedef struct symbol
{
  unsigned short type;
  struct array *name;
  struct cons *plist;
  struct package *home_package;
  struct cons *value;
  struct function *fun;
}__attribute__((packed)) symbol;

typedef struct hash_table
{
  unsigned short type;
  struct cons *plist;
  struct cons **table;
}__attribute__((packed)) hash_table;

typedef struct package
{
  unsigned short type;
  struct cons *plist;
  struct array *name;
  struct array *global;//Change to hash table one day
}__attribute__((packed)) package;

typedef struct procinfo//Global stuff for each Lisp 'process'.
{
  unsigned short type;
  struct symbol *package_sym;
  //Just for speed. :)
  //This is the *pakage* binding.
  //The current package can now be swiftly looked-up, without having to look up the symbol in :cl.
  struct cons *packages;
  //Packages that are defined for this process. A list unique to each process.
  //common-lisp is always first package, cl-user second, keyword third, followed by the rest.
}__attribute__((packed)) procinfo;

typedef struct stream
{
  unsigned short type;
  struct cons *plist;
  int read_index;
  int write_index;
  struct array *rv;//read vector
  struct array *wv;//write vector
}__attribute__((packed)) stream;

typedef struct composite_stream
{
  unsigned short type;
  struct cons *plist;
  struct cons *streams;
}__attribute__((packed)) composite_stream;

#endif

extern cons *nil;
extern cons *t;

extern procinfo *init();
extern cons *newcons();
extern fixnum *newfixnum(long i); 
extern bignum *newbignum();
extern ratio *newratio(fixnum *n, fixnum *d);
extern single *newsingle();
extern base_char *newbase_char();
extern array *newarray(long rank, int length);
extern array *newsimple_vector(int size);
extern package *newpackage();
extern compiled_function *newcompiled_function();
extern base_char *ctolc(char c);
extern array *strtolstr(char *str);
extern cons *null (cons *a);
extern cons *numberp(cons *a);
extern cons *fcons(cons *a, cons *b);
extern cons *car(cons *a);
extern cons *rplaca(cons *a, cons *new);
extern cons *cdr(cons *a);
extern cons *rplacd(cons *a, cons *new);
extern symbol *intern(array *name, package *p);
extern cons *chareq(base_char *a, base_char *b);
extern cons *charequal(base_char *a, base_char *b);
extern cons *stringeq(array *a, array *b);
extern cons *stringequal(array *a, array *b);
extern package *find_package(array *name, procinfo *pinfo);
extern cons *eq (cons *a, cons *b);
extern cons *eql (cons *a, cons *b);
extern cons *equal (cons *a, cons *b);
extern cons *lookup(char *name, cons *env);
extern cons *mkpair(cons *key, cons *value);
extern cons *eval(cons *exp, cons *env);
extern cons *extend_env(cons *env);
extern cons *envbind(cons *key, cons *value, cons *env);
//extern cons *evalopt(cons **lambda_list, cons **args, cons **newenv);
extern cons *evalambda(cons *lambda_list, cons *args, cons *env);
extern symbol *defun(symbol *sym, cons *lambda_list, cons *form, cons *env);
extern cons *assoc(cons *key, cons *plist);
