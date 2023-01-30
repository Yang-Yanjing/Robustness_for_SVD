}
static const char *ParseModulation(const char *str)
{
     static const struct mod
     {
         char dvb[9];
         char vlc[7];
     } tab[] = {
         { "APSK_16", "16APSK" }, { "APSK_32", "32APSK" },
         { "DQPSK", "DQPSK" }, { "PSK_8", "8PSK" }, { "QPSK", "QPSK" },
         { "QAM_128", "128QAM" }, { "QAM_16", "16QAM" },
         { "QAM_256", "256QAM" }, { "QAM_32", "32QAM" },
         { "QAM_64", "64QAM" }, { "QAM_AUTO", "QAM" },
         { "VSB_16", "16VSB" }, { "VSB_8", "8VSB" }
     };
     const struct mod *m = bsearch(str, tab, sizeof (tab) / sizeof(tab[0]),
                                   sizeof (tab[0]), cmp);
     return (m != NULL) ? m->vlc : NULL;
}
