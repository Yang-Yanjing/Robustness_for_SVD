}
int BufferPrintf( buffer_t *p_buffer, const char *p_fmt, ... )
{
    int i_len;
    int status;
    char *psz_data;
    va_list va;
    va_start( va, p_fmt );
    i_len = vasprintf( &psz_data, p_fmt, va );
    va_end( va );
    if( i_len == -1 )
        return VLC_ENOMEM;
    status = BufferAdd( p_buffer, psz_data, i_len );
    free( psz_data );
    return status;
}
