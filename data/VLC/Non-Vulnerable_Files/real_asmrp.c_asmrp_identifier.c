}
static void asmrp_identifier (asmrp_t *p) {
  int l;
  l = 0;
  while ( ((p->ch>='A') && (p->ch<='z'))
      || ((p->ch>='0') && (p->ch<='9'))) {
    p->str[l] = p->ch;
    l++;
    asmrp_getch (p);
  }
  p->str[l]=0;
  p->sym = ASMRP_SYM_ID;
}
