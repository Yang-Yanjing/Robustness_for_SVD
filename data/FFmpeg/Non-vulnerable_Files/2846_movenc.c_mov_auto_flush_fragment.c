static int mov_auto_flush_fragment(AVFormatContext *s)
{
    MOVMuxContext *mov = s->priv_data;
    int had_moov = mov->moov_written;
    int ret = mov_flush_fragment(s);
    if (ret < 0)
        return ret;
    
    
    if (!had_moov && mov->flags & FF_MOV_FLAG_DELAY_MOOV)
        ret = mov_flush_fragment(s);
    return ret;
}
