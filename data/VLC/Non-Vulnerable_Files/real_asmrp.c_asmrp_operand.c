static int asmrp_condition (asmrp_t *p) ;
static int asmrp_operand (asmrp_t *p) {
  int i, ret;
  lprintf ("operand\n");
  ret = 0;
  switch (p->sym) {
  case ASMRP_SYM_DOLLAR:
    asmrp_get_sym (p);
    if (p->sym != ASMRP_SYM_ID) {
      printf ("error: identifier expected.\n");
      break;
    }
    i = asmrp_find_id (p, p->str);
    if (i<0) {
      lprintf ("error: unknown identifier %s\n", p->str);
    }
    ret = p->sym_tab[i].v;
    asmrp_get_sym (p);
    break;
  case ASMRP_SYM_NUM:
    ret = p->num;
    asmrp_get_sym (p);
    break;
  case ASMRP_SYM_LPAREN:
    asmrp_get_sym (p);
    ret = asmrp_condition (p);
    if (p->sym != ASMRP_SYM_RPAREN) {
      printf ("error: ) expected.\n");
      break;
    }
    asmrp_get_sym (p);
    break;
  default:
    lprintf ("syntax error, $ number or ( expected\n");
    break;
  }
  lprintf ("operand done, =%d\n", ret);
  return ret;
}
