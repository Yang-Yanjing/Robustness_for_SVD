 *****************************************************************************/
static ssize_t Write(sout_access_out_t *p_access, block_t *p_buffer)
{
    sout_access_out_sys_t *p_sys = (sout_access_out_sys_t*)p_access->p_sys;
    size_t i_write = 0;
    int val;
    while (p_buffer != NULL) {
        block_t *p_next = p_buffer->p_next;
#if LIBAVFORMAT_VERSION_MAJOR < 54
        val = url_write(p_sys->context, p_buffer->p_buffer, p_buffer->i_buffer);
        if (val < 0)
            goto error;
        i_write += val;
#else
        avio_write(p_sys->context, p_buffer->p_buffer, p_buffer->i_buffer);
        avio_flush(p_sys->context);
        if ((val = p_sys->context->error) != 0) {
            p_sys->context->error = 0; /* FIXME? */
            goto error;
        }
        i_write += p_buffer->i_buffer;
#endif
        block_Release(p_buffer);
        p_buffer = p_next;
    }
    return i_write;
error:
    msg_Err(p_access, "Wrote only %zu bytes: %s", i_write,
            vlc_strerror_c(AVUNERROR(val)));
    block_ChainRelease( p_buffer );
    return i_write;
}
