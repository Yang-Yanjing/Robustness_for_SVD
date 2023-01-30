}
static void asmrp_assignment (asmrp_t *p) {
  lprintf ("assignment\n");
  if (p->sym == ASMRP_SYM_COMMA || p->sym == ASMRP_SYM_SEMICOLON) {
    lprintf ("empty assignment\n");
    return;
  }
  if (p->sym != ASMRP_SYM_ID) {
    printf ("error: identifier expected\n");
    return;
  }
  asmrp_get_sym (p);
  if (p->sym != ASMRP_SYM_EQUALS) {
    printf ("error: = expected\n");
    return;
  }
  asmrp_get_sym (p);
  if ( (p->sym != ASMRP_SYM_NUM) && (p->sym != ASMRP_SYM_STRING)
       && (p->sym != ASMRP_SYM_ID)) {
    printf ("error: number or string expected\n");
    return;
  }
  asmrp_get_sym (p);
  lprintf ("assignment done\n");
}
}
static void asmrp_assignment (asmrp_t *p) {
  lprintf ("assignment\n");
  if (p->sym == ASMRP_SYM_COMMA || p->sym == ASMRP_SYM_SEMICOLON) {
    lprintf ("empty assignment\n");
    return;
  }
  if (p->sym != ASMRP_SYM_ID) {
    printf ("error: identifier expected\n");
    return;
  }
  asmrp_get_sym (p);
  if (p->sym != ASMRP_SYM_EQUALS) {
    printf ("error: = expected\n");
    return;
  }
  asmrp_get_sym (p);
  if ( (p->sym != ASMRP_SYM_NUM) && (p->sym != ASMRP_SYM_STRING)
       && (p->sym != ASMRP_SYM_ID)) {
    printf ("error: number or string expected\n");
    return;
  }
  asmrp_get_sym (p);
  lprintf ("assignment done\n");
}
