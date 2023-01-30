DEFINE_WRITER_CLASS(compact);
static av_cold int compact_init(WriterContext *wctx)
{
    CompactContext *compact = wctx->priv;
    if (strlen(compact->item_sep_str) != 1) {
        av_log(wctx, AV_LOG_ERROR, "Item separator '%s' specified, but must contain a single character\n",
               compact->item_sep_str);
        return AVERROR(EINVAL);
    }
    compact->item_sep = compact->item_sep_str[0];
    if      (!strcmp(compact->escape_mode_str, "none")) compact->escape_str = none_escape_str;
    else if (!strcmp(compact->escape_mode_str, "c"   )) compact->escape_str = c_escape_str;
    else if (!strcmp(compact->escape_mode_str, "csv" )) compact->escape_str = csv_escape_str;
    else {
        av_log(wctx, AV_LOG_ERROR, "Unknown escape mode '%s'\n", compact->escape_mode_str);
        return AVERROR(EINVAL);
    }
    return 0;
}
