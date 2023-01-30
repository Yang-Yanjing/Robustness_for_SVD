}
static int IgnoreBlock(stream_t *s, int block)
{
    /* */
    rar_block_t bk;
    if (PeekBlock(s, &bk) || bk.type != block)
        return VLC_EGENERIC;
    return SkipBlock(s, &bk);
}
