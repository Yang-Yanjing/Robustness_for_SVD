}
static const char *ParseGuard(const char *str)
{
     static const struct guard
     {
         char dvb[7];
         char vlc[7];
     } tab[] = {
         { "19_128", "19/128" }, { "19_256", "19/256" }, { "1_128", "1/128" },
         { "1_16", "1/16" }, { "1_32", "1/32" }, { "1_4", "1/4" },
         { "1_8", "1/8" }, { "AUTO", "" },
     };
     if (strncmp(str, "GUARD_INTERVAL_", 15))
         return NULL;
     str += 15;
     const struct guard *g = bsearch(str, tab, sizeof (tab) / sizeof(tab[0]),
                                     sizeof (tab[0]), cmp);
     return (g != NULL) ? g->vlc : NULL;
}
