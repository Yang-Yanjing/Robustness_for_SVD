int av_hash_get_size(const AVHashContext *ctx)
{
    return hashdesc[ctx->type].size;
}
