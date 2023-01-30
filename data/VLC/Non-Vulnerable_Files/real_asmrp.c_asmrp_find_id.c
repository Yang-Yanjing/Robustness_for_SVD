}
static int asmrp_find_id (asmrp_t *p, const char *s) {
  int i;
  for (i=0; i<p->sym_tab_num; i++) {
    if (!strcmp (s, p->sym_tab[i].id))
      return i;
  }
  return -1;
}
