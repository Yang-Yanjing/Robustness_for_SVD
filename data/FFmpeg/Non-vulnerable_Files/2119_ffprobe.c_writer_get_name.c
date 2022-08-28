static const char *writer_get_name(void *p)
{
    WriterContext *wctx = p;
    return wctx->writer->name;
}
