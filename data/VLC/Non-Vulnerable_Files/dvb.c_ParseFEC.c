}
static const char *ParseFEC(const char *str)
{
     static const struct fec
     {
         char dvb[5];
         char vlc[5];
     } tab[] = {
         { "1_2", "1/2" }, { "2_3", "2/3" }, { "3_4", "3/4" },
         { "4_5", "4/5" }, { "5_6", "5/6" }, { "6_7", "6/7" },
         { "7_8", "7/8" }, { "8_9", "8/9" }, { "9_10", "9/10" },
         { "AUTO", "" },   { "NONE", "0" }
     };
     if (strncmp(str, "FEC_", 4))
         return NULL;
     str += 4;
     const struct fec *f = bsearch(str, tab, sizeof (tab) / sizeof(tab[0]),
                                   sizeof (tab[0]), cmp);
     return (f != NULL) ? f->vlc : NULL;
}
