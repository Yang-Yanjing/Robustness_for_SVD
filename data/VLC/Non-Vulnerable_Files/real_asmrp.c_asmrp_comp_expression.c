}
static int asmrp_comp_expression (asmrp_t *p) {
  int a;
  lprintf ("comp_expression\n");
  a = asmrp_operand (p);
  while ( (p->sym == ASMRP_SYM_LESS)
      || (p->sym == ASMRP_SYM_LEQ)
      || (p->sym == ASMRP_SYM_EQUALS)
      || (p->sym == ASMRP_SYM_GEQ)
      || (p->sym == ASMRP_SYM_GREATER) ) {
    int op = p->sym;
    int b;
    asmrp_get_sym (p);
    b = asmrp_operand (p);
    switch (op) {
    case ASMRP_SYM_LESS:
      a = a<b;
      break;
    case ASMRP_SYM_LEQ:
      a = a<=b;
      break;
    case ASMRP_SYM_EQUALS:
      a = a==b;
      break;
    case ASMRP_SYM_GEQ:
      a = a>=b;
      break;
    case ASMRP_SYM_GREATER:
      a = a>b;
      break;
    }
  }
  lprintf ("comp_expression done = %d\n", a);
  return a;
}
