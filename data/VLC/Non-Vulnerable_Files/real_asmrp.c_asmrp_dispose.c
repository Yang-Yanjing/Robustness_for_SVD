}
static void asmrp_dispose (asmrp_t *p) {
  int i;
  for (i=0; i<p->sym_tab_num; i++)
    free (p->sym_tab[i].id);
  free( p->buf );
  free( p );
}
