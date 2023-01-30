}
static int64_t IOSeek( void *opaque, int64_t offset, int whence )
{
    sout_mux_t *p_mux = opaque;
#ifdef AVFORMAT_DEBUG
    msg_Dbg( p_mux, "IOSeek offset: %"PRId64", whence: %i", offset, whence );
#endif
    switch( whence )
    {
    case SEEK_SET:
        return sout_AccessOutSeek( p_mux->p_access, offset );
    case SEEK_CUR:
    case SEEK_END:
    default:
        return -1;
    }
}
