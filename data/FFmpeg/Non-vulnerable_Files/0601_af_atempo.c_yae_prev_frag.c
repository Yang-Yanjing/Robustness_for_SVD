inline static AudioFragment *yae_prev_frag(ATempoContext *atempo)
{
    return &atempo->frag[(atempo->nfrag + 1) % 2];
}
