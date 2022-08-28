static float get_bsize(ShowFreqsContext *s, int f)
{
    switch (s->fscale) {
    case FS_LINEAR:
        return s->w/(float)s->nb_freq;
    case FS_LOG:
        return pow(s->w, (s->nb_freq-f-1)/(s->nb_freq-1.))-
               pow(s->w, (s->nb_freq-f-2)/(s->nb_freq-1.));
    case FS_RLOG:
        return pow(s->w, (f+1)/(s->nb_freq-1.))-
               pow(s->w,  f   /(s->nb_freq-1.));
    }
    return 1.;
}
