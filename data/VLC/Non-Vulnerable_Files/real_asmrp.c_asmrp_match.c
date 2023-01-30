}
int asmrp_match (const char *rules, int bandwidth, int *matches, int matchsize) {
  asmrp_t *p;
  int      num_matches;
  p = asmrp_new ();
  asmrp_init (p, rules);
  asmrp_set_id (p, "Bandwidth", bandwidth);
  asmrp_set_id (p, "OldPNMPlayer", 0);
  num_matches = asmrp_eval (p, matches, matchsize);
  asmrp_dispose (p);
  return num_matches;
}
