}
static void Close(vlc_object_t *p_this)
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    es_format_Clean(&p_dec->fmt_out);
    block_BytestreamRelease(&p_sys->bytestream);
    free(p_sys);
}
