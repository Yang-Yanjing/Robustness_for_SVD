static int Demux( demux_t *p_demux);
static int StoreString( demux_t *p_demux, char **ppsz_string,
                        const char *psz_source_start,
                        const char *psz_source_end )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    unsigned len = psz_source_end - psz_source_start;
    free( *ppsz_string );
    char *buf = *ppsz_string = malloc ((len * (1 + !p_sys->b_utf8)) + 1);
    if (buf == NULL)
        return VLC_ENOMEM;
    if( p_sys->b_utf8 )
    {
        memcpy (buf, psz_source_start, len);
        (*ppsz_string)[len] = '\0';
        EnsureUTF8 (*ppsz_string);
    }
    else
    {
        /* Latin-1 -> UTF-8 */
        for (unsigned i = 0; i < len; i++)
        {
            unsigned char c = psz_source_start[i];
            if (c & 0x80)
            {
                *buf++ = 0xc0 | (c >> 6);
                *buf++ = 0x80 | (c & 0x3f);
            }
            else
                *buf++ = c;
        }
        *buf++ = '\0';
        buf = realloc (*ppsz_string, buf - *ppsz_string);
        if( buf )
            *ppsz_string = buf;
    }
    return VLC_SUCCESS;
}
