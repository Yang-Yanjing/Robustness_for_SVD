}
static int asmrp_rule (asmrp_t *p) {
  int ret;
  lprintf ("rule\n");
  ret = 1;
  if (p->sym == ASMRP_SYM_HASH) {
    asmrp_get_sym (p);
    ret = asmrp_condition (p);
    while (p->sym == ASMRP_SYM_COMMA) {
      asmrp_get_sym (p);
      asmrp_assignment (p);
    }
  } else if (p->sym != ASMRP_SYM_SEMICOLON) {
    asmrp_assignment (p);
    while (p->sym == ASMRP_SYM_COMMA) {
      asmrp_get_sym (p);
      asmrp_assignment (p);
    }
  }
  lprintf ("rule done = %d\n", ret);
  if (p->sym != ASMRP_SYM_SEMICOLON) {
    printf ("semicolon expected.\n");
    return ret;
  }
  asmrp_get_sym (p);
  return ret;
}
