DEFINE_WRITER_CLASS(flat);
static av_cold int flat_init(WriterContext *wctx)
{
    FlatContext *flat = wctx->priv;
    if (strlen(flat->sep_str) != 1) {
        av_log(wctx, AV_LOG_ERROR, "Item separator '%s' specified, but must contain a single character\n",
               flat->sep_str);
        return AVERROR(EINVAL);
    }
    flat->sep = flat->sep_str[0];
    return 0;
}
