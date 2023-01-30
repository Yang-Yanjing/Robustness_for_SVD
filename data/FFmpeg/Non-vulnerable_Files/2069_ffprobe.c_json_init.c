DEFINE_WRITER_CLASS(json);
static av_cold int json_init(WriterContext *wctx)
{
    JSONContext *json = wctx->priv;
    json->item_sep       = json->compact ? ", " : ",\n";
    json->item_start_end = json->compact ? " "  : "\n";
    return 0;
}
