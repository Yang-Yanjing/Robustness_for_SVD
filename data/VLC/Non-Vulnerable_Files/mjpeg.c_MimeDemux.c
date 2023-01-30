}
static int MimeDemux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int         i_size, i;
    bool  b_match = CheckMimeHeader( p_demux, &i_size );
    if( i_size > 0 )
    {
        if( stream_Read( p_demux->s, NULL, i_size ) != i_size )
            return 0;
    }
    else if( i_size < 0 )
    {
        return 0;
    }
    else
    {
        // No MIME header, assume OK
        b_match = true;
    }
    if( !Peek( p_demux, true ) )
    {
        msg_Warn( p_demux, "cannot peek data" );
        return 0;
    }
    i = 0;
    i_size = strlen( p_sys->psz_separator ) + 2;
    if( p_sys->i_data_peeked < i_size )
    {
        msg_Warn( p_demux, "data shortage" );
        return 0;
    }
    for( ;; )
    {
        while( !( p_sys->p_peek[i] == '-' && p_sys->p_peek[i+1] == '-' ) )
        {
            i++;
            i_size++;
            if( i_size >= p_sys->i_data_peeked )
            {
                msg_Dbg( p_demux, "MIME boundary not found in %d bytes of "
                         "data", p_sys->i_data_peeked );
                if( !Peek( p_demux, false ) )
                {
                    msg_Warn( p_demux, "no more data is available at the "
                              "moment" );
                    return 0;
                }
            }
        }
        /* Handle old and new style of separators */
        if (!strncmp(p_sys->psz_separator, (char *)(p_sys->p_peek + i + 2),
                     strlen( p_sys->psz_separator ))
         || ((strlen(p_sys->psz_separator) > 4)
          && !strncmp(p_sys->psz_separator, "--", 2)
          && !strncmp(p_sys->psz_separator, (char *)(p_sys->p_peek + i),
                      strlen( p_sys->psz_separator))))
        {
            break;
        }
        i++;
        i_size++;
    }
    if( !b_match )
    {
        msg_Err( p_demux, "discard non-JPEG part" );
        stream_Read( p_demux->s, NULL, i );
        return 0;
    }
    return SendBlock( p_demux, i );
}
