extern cons *lnull(cons *env);
extern cons *lnumberp(cons *env);
extern cons *lcons(cons *env);
extern cons *lcar(cons *env);
extern cons *lrplaca(cons *env);
extern cons *lcdr(cons *env);
extern cons *lrplacd(cons *env);
extern symbol *lintern(cons *env);
extern package *lfind_package(cons *env);

extern cons *lchareq(cons *env);
extern cons *lcharequal(cons *env);
extern cons *lstringeq(cons *env);
extern cons *lstringequal(cons *env);

extern cons *leq (cons *env);
extern cons *leql (cons *env);
extern cons *lequal (cons *env);
extern cons *lcond (cons *env);
extern cons *lquote(cons *env);
extern cons *leval(cons *env);

extern cons *ldefun(cons *env);
extern cons* lsetq(cons *env);
