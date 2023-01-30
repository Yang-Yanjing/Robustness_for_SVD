const char *av_hash_get_name(const AVHashContext *ctx)
{
    return hashdesc[ctx->type].name;
}
