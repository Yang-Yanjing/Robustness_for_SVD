}
static void asmrp_init (asmrp_t *p, const char *str) {
  p->buf = strdup (str);
  p->pos = 0;
  asmrp_getch (p);
}
