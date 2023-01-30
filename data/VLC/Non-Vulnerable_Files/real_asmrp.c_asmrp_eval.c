}
static int asmrp_eval (asmrp_t *p, int *matches, int matchsize) {
  int rule_num, num_matches;
  lprintf ("eval\n");
  asmrp_get_sym (p);
  rule_num = 0; num_matches = 0;
  while (p->sym != ASMRP_SYM_EOF && num_matches < matchsize - 1) {
    if (asmrp_rule (p)) {
      lprintf ("rule #%d is true\n", rule_num);
      matches[num_matches] = rule_num;
      num_matches++;
    }
    rule_num++;
  }
  matches[num_matches] = -1;
  return num_matches;
}
