}
static void asmrp_string (asmrp_t *p) {
  int l;
  l = 0;
  while ( (p->ch!='"') && (p->ch>=32) ) {
    p->str[l] = p->ch;
    l++;
    asmrp_getch (p);
  }
  p->str[l]=0;
  if (p->ch=='"')
    asmrp_getch (p);
  p->sym = ASMRP_SYM_STRING;
}
