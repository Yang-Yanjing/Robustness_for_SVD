}
static int asmrp_set_id (asmrp_t *p, const char *s, int v) {
  int i;
  i = asmrp_find_id (p, s);
  if (i<0) {
    i = p->sym_tab_num;
    p->sym_tab_num++;
    p->sym_tab[i].id = strdup (s);
    lprintf ("new symbol '%s'\n", s);
  }
  p->sym_tab[i].v = v;
  lprintf ("symbol '%s' assigned %d\n", s, v);
  return i;
}
