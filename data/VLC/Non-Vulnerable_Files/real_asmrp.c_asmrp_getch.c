}
static void asmrp_getch (asmrp_t *p) {
  p->ch = p->buf[p->pos];
  p->pos++;
  lprintf ("%c\n", p->ch);
}
