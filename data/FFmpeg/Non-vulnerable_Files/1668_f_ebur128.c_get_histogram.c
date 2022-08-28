static struct hist_entry *get_histogram(void)
{
    int i;
    struct hist_entry *h = av_calloc(HIST_SIZE, sizeof(*h));
    if (!h)
        return NULL;
    for (i = 0; i < HIST_SIZE; i++) {
        h[i].loudness = i / (double)HIST_GRAIN + ABS_THRES;
        h[i].energy   = ENERGY(h[i].loudness);
    }
    return h;
}
