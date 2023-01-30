 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    int i_error = VLC_SUCCESS;
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    const uint8_t *p_peek;
    if( stream_Peek( p_demux->s, &p_peek, 8 ) < 8 )
        return VLC_EGENERIC;
    /* Is it a caf file? */
    if( memcmp( p_peek, "caff", 4 ))
        return VLC_EGENERIC;
    /* check file version (we only handle version 1) */
    uint16_t i_version = GetWBE( p_peek + 4 );
    if( i_version != 1 )
    {
        msg_Dbg( p_demux, "Unknown caf file version %d.", i_version );
        return VLC_EGENERIC;
    }
    /* check file flags (must be 0) */
    uint16_t i_flags = GetWBE( p_peek + 6 );
    if( i_flags != 0 )
    {
        msg_Dbg( p_demux, "Unknown caf file flags %d.", i_flags );
        return VLC_EGENERIC;
    }
    if( stream_Read( p_demux->s, NULL, 8 ) < 8 )
        return VLC_EGENERIC; /* This would be very strange since we justed peeked at these bytes. */
    p_demux->p_sys = calloc( 1, sizeof( demux_sys_t ));
    if( !p_demux->p_sys ) return VLC_ENOMEM;
    /* From this point on, we have to free p_sys if we return an error (e.g. "goto caf_open_end") */
    p_sys = p_demux->p_sys;
    es_format_Init( &p_sys->fmt, UNKNOWN_ES, 0 );
    vlc_fourcc_t i_fcc;
    uint64_t i_size;
    uint64_t i_idx = 0;
    while( NextChunk( p_demux, &i_fcc, &i_size ) == VLC_SUCCESS )
    {
        bool b_handled = true;
        switch ( i_fcc )
        {
            case VLC_FOURCC( 'd', 'e', 's', 'c' ):
                if( i_idx != 0 )
                {
                    msg_Err( p_demux, "The audio description chunk must be the first chunk in a caf file." );
                    i_error = VLC_EGENERIC;
                    goto caf_open_end;
                }
                i_error = ReadDescChunk( p_demux );
                break;
            case VLC_FOURCC( 'd', 'a', 't', 'a' ):
                i_error = ReadDataChunk( p_demux, i_size );
                break;
            case VLC_FOURCC( 'p', 'a', 'k', 't' ):
                i_error = ReadPaktChunk( p_demux );
                break;
            case VLC_FOURCC( 'k', 'u', 'k', 'i' ):
                i_error = ReadKukiChunk( p_demux, i_size );
                break;
            default:
                b_handled = false;
                break;
        }
        if( i_error )
            goto caf_open_end;
        if( b_handled )
            msg_Dbg( p_demux, "Found '%4.4s' chunk.", ( char * )&i_fcc );
        else
            msg_Dbg( p_demux, "Ignoring '%4.4s' chunk.", ( char * )&i_fcc );
        if( i_size == kCHUNK_SIZE_EOF )
            break;
        if( stream_Seek( p_demux->s, stream_Tell( p_demux->s ) + i_size ) != VLC_SUCCESS )
            break;
        i_idx++;
    }
    if ( !p_sys->i_data_offset || p_sys->fmt.i_cat != AUDIO_ES ||
        ( NeedsPacketTable( p_sys ) && !p_sys->packet_table.i_descriptions_start ))
    {
        msg_Err( p_demux, "Did not find all necessary chunks." );
        i_error = VLC_EGENERIC;
        goto caf_open_end;
    }
    p_sys->es = es_out_Add( p_demux->out, &p_sys->fmt );
    if( !p_sys->es )
    {
        msg_Err( p_demux, "Could not add elementary stream." );
        i_error = VLC_EGENERIC;
        goto caf_open_end;
    }
caf_open_end:
    if( i_error )
    {
        free( p_sys->fmt.p_extra );
        free( p_sys  );
        if( stream_Seek( p_demux->s, 0 ))
        {
            msg_Warn(p_demux, "Could not reset stream position to 0.");
        }
    }
    else
    {
        p_demux->pf_control = Control;
        p_demux->pf_demux = Demux;
    }
    return i_error;
}