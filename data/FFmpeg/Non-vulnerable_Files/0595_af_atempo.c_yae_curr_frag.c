AVFILTER_DEFINE_CLASS(atempo);
inline static AudioFragment *yae_curr_frag(ATempoContext *atempo)
{
    return &atempo->frag[atempo->nfrag % 2];
}
