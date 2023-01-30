 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    uint8_t     header[5];
    const uint8_t *p_peek;
    int         i_size;
    block_t     *p_frame;
    for( ;; )
    {
        if( stream_Peek( p_demux->s, &p_peek, 8 ) < 8 )
        {
            msg_Warn( p_demux, "cannot peek" );
            return 0;
        }
        if( !memcmp( p_peek, "NSVf", 4 ) )
        {
            if( ReadNSVf( p_demux ) )
                return -1;
        }
        else if( !memcmp( p_peek, "NSVs", 4 ) )
        {
            if( p_sys->b_start_record )
            {
                /* Enable recording once synchronized */
                stream_Control( p_demux->s, STREAM_SET_RECORD_STATE, true, "nsv" );
                p_sys->b_start_record = false;
            }
            if( ReadNSVs( p_demux ) )
                return -1;
            break;
        }
        else if( GetWLE( p_peek ) == 0xbeef )
        {
            /* Next frame of the current NSVs chunk */
            if( stream_Read( p_demux->s, NULL, 2 ) < 2 )
            {
                msg_Warn( p_demux, "cannot read" );
                return 0;
            }
            break;
        }
        else
        {
            msg_Err( p_demux, "invalid signature 0x%x (%4.4s)", GetDWLE( p_peek ), (const char*)p_peek );
            if( ReSynch( p_demux ) )
                return -1;
        }
    }
    if( stream_Read( p_demux->s, header, 5 ) < 5 )
    {
        msg_Warn( p_demux, "cannot read" );
        return 0;
    }
    /* Set PCR */
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, VLC_TS_0 + p_sys->i_pcr );
    /* Read video */
    i_size = ( header[0] >> 4 ) | ( header[1] << 4 ) | ( header[2] << 12 );
    if( i_size > 0 )
    {
        /* extra data ? */
        if( (header[0]&0x0f) != 0x0 )
        {
            uint8_t      aux[6];
            int          i_aux;
            vlc_fourcc_t fcc;
            if( stream_Read( p_demux->s, aux, 6 ) < 6 )
            {
                msg_Warn( p_demux, "cannot read" );
                return 0;
            }
            i_aux = GetWLE( aux );
            fcc   = VLC_FOURCC( aux[2], aux[3], aux[4], aux[5] );
            msg_Dbg( p_demux, "Belekas: %d - size=%d fcc=%4.4s",
                     header[0]&0xf, i_aux, (char*)&fcc );
            if( fcc == VLC_FOURCC( 'S', 'U', 'B', 'T' ) && i_aux > 2 )
            {
                if( p_sys->p_sub == NULL )
                {
                    p_sys->fmt_sub.i_codec = VLC_FOURCC( 's', 'u', 'b', 't' );
                    p_sys->p_sub = es_out_Add( p_demux->out, &p_sys->fmt_sub );
                    es_out_Control( p_demux->out, ES_OUT_SET_ES, p_sys->p_sub );
                }
                stream_Read( p_demux->s, NULL, 2 );
                if( ( p_frame = stream_Block( p_demux->s, i_aux - 2 ) ) )
                {
                    uint8_t *p = p_frame->p_buffer;
                    while( p < &p_frame->p_buffer[p_frame->i_buffer] && *p != 0 )
                    {
                        p++;
                    }
                    if( *p == 0 && p + 1 < &p_frame->p_buffer[p_frame->i_buffer] )
                    {
                        p_frame->i_buffer -= p + 1 - p_frame->p_buffer;
                        p_frame->p_buffer = p + 1;
                    }
                    /* Skip the first part (it is the language name) */
                    p_frame->i_pts = VLC_TS_0 + p_sys->i_pcr;
                    p_frame->i_dts = VLC_TS_0 + p_sys->i_pcr + 4000000;    /* 4s */
                    es_out_Send( p_demux->out, p_sys->p_sub, p_frame );
                }
            }
            else
            {
                /* We skip this extra data */
                if( stream_Read( p_demux->s, NULL, i_aux ) < i_aux )
                {
                    msg_Warn( p_demux, "cannot read" );
                    return 0;
                }
            }
            i_size -= 6 + i_aux;
        }
        /* msg_Dbg( p_demux, "frame video size=%d", i_size ); */
        if( i_size > 0 && ( p_frame = stream_Block( p_demux->s, i_size ) ) )
        {
            p_frame->i_dts = VLC_TS_0 + p_sys->i_pcr;
            es_out_Send( p_demux->out, p_sys->p_video, p_frame );
        }
    }
    /* Read audio */
    i_size = header[3] | ( header[4] << 8 );
    if( i_size > 0 )
    {
        /* msg_Dbg( p_demux, "frame audio size=%d", i_size ); */
        if( p_sys->fmt_audio.i_codec == VLC_FOURCC( 'a', 'r', 'a', 'w' ) )
        {
            uint8_t h[4];
            stream_Read( p_demux->s, h, 4 );
            p_sys->fmt_audio.audio.i_channels = h[1];
            p_sys->fmt_audio.audio.i_rate = GetWLE( &h[2] );
            i_size -= 4;
        }
        if( p_sys->p_audio == NULL )
        {
            p_sys->p_audio = es_out_Add( p_demux->out, &p_sys->fmt_audio );
        }
        if( ( p_frame = stream_Block( p_demux->s, i_size ) ) )
        {
            p_frame->i_dts =
            p_frame->i_pts = VLC_TS_0 + p_sys->i_pcr;
            es_out_Send( p_demux->out, p_sys->p_audio, p_frame );
        }
    }
    p_sys->i_pcr += p_sys->i_pcr_inc;
    if( p_sys->i_time >= 0 )
    {
        p_sys->i_time += p_sys->i_pcr_inc;
    }
    return 1;
}
