#endif
static void asmrp_get_sym (asmrp_t *p) {
  while (p->ch <= 32) {
    if (p->ch == 0) {
      p->sym = ASMRP_SYM_EOF;
      return;
    }
    asmrp_getch (p);
  }
  if (p->ch == '\\')
    asmrp_getch (p);
  switch (p->ch) {
  case '#':
    p->sym = ASMRP_SYM_HASH;
    asmrp_getch (p);
    break;
  case ';':
    p->sym = ASMRP_SYM_SEMICOLON;
    asmrp_getch (p);
    break;
  case ',':
    p->sym = ASMRP_SYM_COMMA;
    asmrp_getch (p);
    break;
  case '=':
    p->sym = ASMRP_SYM_EQUALS;
    asmrp_getch (p);
    if (p->ch=='=')
      asmrp_getch (p);
    break;
  case '&':
    p->sym = ASMRP_SYM_AND;
    asmrp_getch (p);
    if (p->ch=='&')
      asmrp_getch (p);
    break;
  case '|':
    p->sym = ASMRP_SYM_OR;
    asmrp_getch (p);
    if (p->ch=='|')
      asmrp_getch (p);
    break;
  case '<':
    p->sym = ASMRP_SYM_LESS;
    asmrp_getch (p);
    if (p->ch=='=') {
      p->sym = ASMRP_SYM_LEQ;
      asmrp_getch (p);
    }
    break;
  case '>':
    p->sym = ASMRP_SYM_GREATER;
    asmrp_getch (p);
    if (p->ch=='=') {
      p->sym = ASMRP_SYM_GEQ;
      asmrp_getch (p);
    }
    break;
  case '$':
    p->sym = ASMRP_SYM_DOLLAR;
    asmrp_getch (p);
    break;
  case '(':
    p->sym = ASMRP_SYM_LPAREN;
    asmrp_getch (p);
    break;
  case ')':
    p->sym = ASMRP_SYM_RPAREN;
    asmrp_getch (p);
    break;
  case '"':
    asmrp_getch (p);
    asmrp_string (p);
    break;
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    asmrp_number (p);
    break;
  default:
    asmrp_identifier (p);
  }
#ifdef LOG
  asmrp_print_sym (p);
#endif
}
#endif
static void asmrp_get_sym (asmrp_t *p) {
  while (p->ch <= 32) {
    if (p->ch == 0) {
      p->sym = ASMRP_SYM_EOF;
      return;
    }
    asmrp_getch (p);
  }
  if (p->ch == '\\')
    asmrp_getch (p);
  switch (p->ch) {
  case '#':
    p->sym = ASMRP_SYM_HASH;
    asmrp_getch (p);
    break;
  case ';':
    p->sym = ASMRP_SYM_SEMICOLON;
    asmrp_getch (p);
    break;
  case ',':
    p->sym = ASMRP_SYM_COMMA;
    asmrp_getch (p);
    break;
  case '=':
    p->sym = ASMRP_SYM_EQUALS;
    asmrp_getch (p);
    if (p->ch=='=')
      asmrp_getch (p);
    break;
  case '&':
    p->sym = ASMRP_SYM_AND;
    asmrp_getch (p);
    if (p->ch=='&')
      asmrp_getch (p);
    break;
  case '|':
    p->sym = ASMRP_SYM_OR;
    asmrp_getch (p);
    if (p->ch=='|')
      asmrp_getch (p);
    break;
  case '<':
    p->sym = ASMRP_SYM_LESS;
    asmrp_getch (p);
    if (p->ch=='=') {
      p->sym = ASMRP_SYM_LEQ;
      asmrp_getch (p);
    }
    break;
  case '>':
    p->sym = ASMRP_SYM_GREATER;
    asmrp_getch (p);
    if (p->ch=='=') {
      p->sym = ASMRP_SYM_GEQ;
      asmrp_getch (p);
    }
    break;
  case '$':
    p->sym = ASMRP_SYM_DOLLAR;
    asmrp_getch (p);
    break;
  case '(':
    p->sym = ASMRP_SYM_LPAREN;
    asmrp_getch (p);
    break;
  case ')':
    p->sym = ASMRP_SYM_RPAREN;
    asmrp_getch (p);
    break;
  case '"':
    asmrp_getch (p);
    asmrp_string (p);
    break;
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    asmrp_number (p);
    break;
  default:
    asmrp_identifier (p);
  }
#ifdef LOG
  asmrp_print_sym (p);
#endif
}
