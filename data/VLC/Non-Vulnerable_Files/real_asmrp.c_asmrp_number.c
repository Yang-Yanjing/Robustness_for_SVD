}
static void asmrp_number (asmrp_t *p) {
  int num;
  num = 0;
  while ( (p->ch>='0') && (p->ch<='9') ) {
    num = num*10 + (p->ch - '0');
    asmrp_getch (p);
  }
  p->sym = ASMRP_SYM_NUM;
  p->num = num;
}
