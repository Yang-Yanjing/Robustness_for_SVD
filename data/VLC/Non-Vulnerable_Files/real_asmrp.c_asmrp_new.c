} asmrp_t;
static asmrp_t *asmrp_new (void ) {
  asmrp_t *p;
  p = malloc (sizeof (asmrp_t));
  p->sym_tab_num = 0;
  p->sym         = ASMRP_SYM_NONE;
  p->buf         = NULL;
  return p;
}
