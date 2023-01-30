}
static int asmrp_condition (asmrp_t *p) {
  int a;
  lprintf ("condition\n");
  a = asmrp_comp_expression (p);
  while ( (p->sym == ASMRP_SYM_AND) || (p->sym == ASMRP_SYM_OR) ) {
    int op, b;
    op = p->sym;
    asmrp_get_sym (p);
    b = asmrp_comp_expression (p);
    switch (op) {
    case ASMRP_SYM_AND:
      a = a & b;
      break;
    case ASMRP_SYM_OR:
      a = a | b;
      break;
    }
  }
  lprintf ("condition done = %d\n", a);
  return a;
}
