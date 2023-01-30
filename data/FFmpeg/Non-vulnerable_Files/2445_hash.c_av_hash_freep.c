void av_hash_freep(AVHashContext **ctx)
{
    if (*ctx)
        av_freep(&(*ctx)->ctx);
    av_freep(ctx);
}
