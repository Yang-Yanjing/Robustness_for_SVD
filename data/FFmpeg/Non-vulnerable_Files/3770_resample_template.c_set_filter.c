static void SET_TYPE(set_filter)(void *filter0, double *tab, int phase,
                                 int tap_count)
{
    int i;
    FELEM *filter = ((FELEM *)filter0) + phase * tap_count;
    for (i = 0; i < tap_count; i++) {
        DBL_TO_FELEM(filter[i], tab[i]);
    }
}
