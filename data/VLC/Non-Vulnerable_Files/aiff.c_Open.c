 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    const uint8_t *p_peek;
    if( stream_Peek( p_demux->s, &p_peek, 12 ) < 12 )
        return VLC_EGENERIC;
    if( memcmp( p_peek, "FORM", 4 ) || memcmp( &p_peek[8], "AIFF", 4 ) )
        return VLC_EGENERIC;
    /* skip aiff header */
    stream_Read( p_demux->s, NULL, 12 );
    /* Fill p_demux field */
    DEMUX_INIT_COMMON(); p_sys = p_demux->p_sys;
    es_format_Init( &p_sys->fmt, UNKNOWN_ES, 0 );
    p_sys->i_time = 0;
    p_sys->i_ssnd_pos = -1;
    for( ;; )
    {
        if( stream_Peek( p_demux->s, &p_peek, 8 ) < 8 )
            goto error;
        uint32_t i_data_size = GetDWBE( &p_peek[4] );
        uint64_t i_chunk_size = UINT64_C( 8 ) + i_data_size + ( i_data_size & 1 );
        msg_Dbg( p_demux, "chunk fcc=%4.4s size=%" PRIu64 " data_size=%" PRIu32,
            p_peek, i_chunk_size, i_data_size );
        if( !memcmp( p_peek, "COMM", 4 ) )
        {
            if( stream_Peek( p_demux->s, &p_peek, 18+8 ) < 18+8 )
                goto error;
            es_format_Init( &p_sys->fmt, AUDIO_ES, VLC_FOURCC( 't', 'w', 'o', 's' ) );
            p_sys->fmt.audio.i_channels = GetWBE( &p_peek[8] );
            p_sys->fmt.audio.i_bitspersample = GetWBE( &p_peek[14] );
            p_sys->fmt.audio.i_rate     = GetF80BE( &p_peek[16] );
            msg_Dbg( p_demux, "COMM: channels=%d samples_frames=%d bits=%d rate=%d",
                     GetWBE( &p_peek[8] ), GetDWBE( &p_peek[10] ), GetWBE( &p_peek[14] ),
                     GetF80BE( &p_peek[16] ) );
        }
        else if( !memcmp( p_peek, "SSND", 4 ) )
        {
            if( stream_Peek( p_demux->s, &p_peek, 8+8 ) < 8+8 )
                goto error;
            p_sys->i_ssnd_pos = stream_Tell( p_demux->s );
            p_sys->i_ssnd_size = i_data_size;
            p_sys->i_ssnd_offset = GetDWBE( &p_peek[8] );
            p_sys->i_ssnd_blocksize = GetDWBE( &p_peek[12] );
            msg_Dbg( p_demux, "SSND: (offset=%d blocksize=%d)",
                     p_sys->i_ssnd_offset, p_sys->i_ssnd_blocksize );
        }
        if( p_sys->i_ssnd_pos >= 12 && p_sys->fmt.i_cat == AUDIO_ES )
        {
            /* We have found the 2 needed chunks */
            break;
        }
        /* consume chunk data */
        for( ssize_t i_req; i_chunk_size; i_chunk_size -= i_req )
        {
#if SSIZE_MAX < UINT64_MAX
            i_req = __MIN( SSIZE_MAX, i_chunk_size );
#else
            i_req = i_chunk_size;
#endif
            if( stream_Read( p_demux->s, NULL, i_req ) != i_req )
            {
                msg_Warn( p_demux, "incomplete file" );
                goto error;
            }
        }
    }
    p_sys->i_ssnd_start = p_sys->i_ssnd_pos + 16 + p_sys->i_ssnd_offset;
    p_sys->i_ssnd_end   = p_sys->i_ssnd_start + p_sys->i_ssnd_size;
    p_sys->i_ssnd_fsize = p_sys->fmt.audio.i_channels *
                          ((p_sys->fmt.audio.i_bitspersample + 7) / 8);
    if( p_sys->i_ssnd_fsize <= 0 || p_sys->fmt.audio.i_rate == 0 )
    {
        msg_Err( p_demux, "invalid audio parameters" );
        goto error;
    }
    if( p_sys->i_ssnd_size <= 0 )
    {
        /* unknown */
        p_sys->i_ssnd_end = 0;
    }
    /* seek into SSND chunk */
    if( stream_Seek( p_demux->s, p_sys->i_ssnd_start ) )
    {
        msg_Err( p_demux, "cannot seek to data chunk" );
        goto error;
    }
    /* */
    p_sys->es = es_out_Add( p_demux->out, &p_sys->fmt );
    return VLC_SUCCESS;
error:
    free( p_sys );
    return VLC_EGENERIC;
}
