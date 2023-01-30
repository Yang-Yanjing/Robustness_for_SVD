#ifdef LOG
static void asmrp_print_sym (asmrp_t *p) {
  printf ("symbol: ");
  switch (p->sym) {
  case ASMRP_SYM_NONE:
    printf ("NONE\n");
    break;
  case ASMRP_SYM_EOF:
    printf ("EOF\n");
    break;
  case ASMRP_SYM_NUM:
    printf ("NUM %d\n", p->num);
    break;
  case ASMRP_SYM_ID:
    printf ("ID '%s'\n", p->str);
    break;
  case ASMRP_SYM_STRING:
    printf ("STRING \"%s\"\n", p->str);
    break;
  case ASMRP_SYM_HASH:
    printf ("#\n");
    break;
  case ASMRP_SYM_SEMICOLON:
    printf (";\n");
    break;
  case ASMRP_SYM_COMMA:
    printf (",\n");
    break;
  case ASMRP_SYM_EQUALS:
    printf ("==\n");
    break;
  case ASMRP_SYM_AND:
    printf ("&&\n");
    break;
  case ASMRP_SYM_OR:
    printf ("||\n");
    break;
  case ASMRP_SYM_LESS:
    printf ("<\n");
    break;
  case ASMRP_SYM_LEQ:
    printf ("<=\n");
    break;
  case ASMRP_SYM_GEQ:
    printf (">=\n");
    break;
  case ASMRP_SYM_GREATER:
    printf (">\n");
    break;
  case ASMRP_SYM_DOLLAR:
    printf ("$\n");
    break;
  case ASMRP_SYM_LPAREN:
    printf ("(\n");
    break;
  case ASMRP_SYM_RPAREN:
    printf (")\n");
    break;
  default:
    printf ("unknown symbol %d\n", p->sym);
  }
}
#ifdef LOG
static void asmrp_print_sym (asmrp_t *p) {
  printf ("symbol: ");
  switch (p->sym) {
  case ASMRP_SYM_NONE:
    printf ("NONE\n");
    break;
  case ASMRP_SYM_EOF:
    printf ("EOF\n");
    break;
  case ASMRP_SYM_NUM:
    printf ("NUM %d\n", p->num);
    break;
  case ASMRP_SYM_ID:
    printf ("ID '%s'\n", p->str);
    break;
  case ASMRP_SYM_STRING:
    printf ("STRING \"%s\"\n", p->str);
    break;
  case ASMRP_SYM_HASH:
    printf ("#\n");
    break;
  case ASMRP_SYM_SEMICOLON:
    printf (";\n");
    break;
  case ASMRP_SYM_COMMA:
    printf (",\n");
    break;
  case ASMRP_SYM_EQUALS:
    printf ("==\n");
    break;
  case ASMRP_SYM_AND:
    printf ("&&\n");
    break;
  case ASMRP_SYM_OR:
    printf ("||\n");
    break;
  case ASMRP_SYM_LESS:
    printf ("<\n");
    break;
  case ASMRP_SYM_LEQ:
    printf ("<=\n");
    break;
  case ASMRP_SYM_GEQ:
    printf (">=\n");
    break;
  case ASMRP_SYM_GREATER:
    printf (">\n");
    break;
  case ASMRP_SYM_DOLLAR:
    printf ("$\n");
    break;
  case ASMRP_SYM_LPAREN:
    printf ("(\n");
    break;
  case ASMRP_SYM_RPAREN:
    printf (")\n");
    break;
  default:
    printf ("unknown symbol %d\n", p->sym);
  }
}
