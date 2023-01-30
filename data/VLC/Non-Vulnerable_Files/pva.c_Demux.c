 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const uint8_t *p_peek;
    int         i_size;
    block_t     *p_frame;
    int64_t     i_pts;
    int         i_skip;
    if( stream_Peek( p_demux->s, &p_peek, 8 ) < 8 )
    {
        msg_Warn( p_demux, "eof ?" );
        return 0;
    }
    if( p_peek[0] != 'A' || p_peek[1] != 'V' || p_peek[4] != 0x55 )
    {
        msg_Warn( p_demux, "lost synchro" );
        if( ReSynch( p_demux ) )
        {
            return -1;
        }
        if( stream_Peek( p_demux->s, &p_peek, 8 ) < 8 )
        {
            msg_Warn( p_demux, "eof ?" );
            return 0;
        }
    }
    i_size = GetWBE( &p_peek[6] );
    switch( p_peek[2] )
    {
        case 0x01:  /* VideoStream */
            if( p_sys->i_vc < 0 )
            {
                msg_Dbg( p_demux, "first packet for video" );
            }
            else if( ((p_sys->i_vc + 1)&0xff) != p_peek[3] )
            {
                msg_Dbg( p_demux, "packet lost (video)" );
                if( p_sys->p_es )
                {
                    block_ChainRelease( p_sys->p_es );
                    p_sys->p_es = NULL;
                }
            }
            p_sys->i_vc = p_peek[3];
            /* read the PTS and potential extra bytes TODO: make it a bit more optimised */
            i_pts = -1;
            i_skip = 8;
            if( p_peek[5]&0x10 )
            {
                int i_pre = p_peek[5]&0x3;
                if( ( p_frame = stream_Block( p_demux->s, 8 + 4 + i_pre ) ) )
                {
                    i_pts = GetDWBE( &p_frame->p_buffer[8] );
                    if( p_frame->i_buffer > 12 )
                    {
                        p_frame->p_buffer += 12;
                        p_frame->i_buffer -= 12;
                        block_ChainAppend( &p_sys->p_es, p_frame );
                    }
                    else
                    {
                        block_Release( p_frame );
                    }
                }
                i_size -= 4 + i_pre;
                i_skip  = 0;
                /* Set PCR */
                if( ( p_frame = p_sys->p_es ) )
                {
                    if( p_frame->i_pts > VLC_TS_INVALID && !p_sys->b_pcr_audio )
                    {
                        es_out_Control( p_demux->out, ES_OUT_SET_PCR, (int64_t)p_frame->i_pts);
                    }
                    es_out_Send( p_demux->out, p_sys->p_video, p_frame );
                    p_sys->p_es = NULL;
                }
            }
            if( ( p_frame = stream_Block( p_demux->s, i_size + i_skip ) ) )
            {
                p_frame->p_buffer += i_skip;
                p_frame->i_buffer -= i_skip;
                if( i_pts >= 0 )
                    p_frame->i_pts = VLC_TS_0 + i_pts * 100 / 9;
                block_ChainAppend( &p_sys->p_es, p_frame );
            }
            break;
        case 0x02:  /* MainAudioStream */
            if( p_sys->i_ac < 0 )
            {
                msg_Dbg( p_demux, "first packet for audio" );
            }
            else if( ((p_sys->i_ac + 1)&0xff) != p_peek[3] )
            {
                msg_Dbg( p_demux, "packet lost (audio)" );
                if( p_sys->p_pes )
                {
                    block_ChainRelease( p_sys->p_pes );
                    p_sys->p_pes = NULL;
                }
            }
            p_sys->i_ac = p_peek[3];
            if( p_peek[5]&0x10 && p_sys->p_pes )
            {
                ParsePES( p_demux );
            }
            if( ( p_frame = stream_Block( p_demux->s, i_size + 8 ) ) )
            {
                p_frame->p_buffer += 8;
                p_frame->i_buffer -= 8;
                /* XXX this a hack, some streams aren't compliant and
                 * don't set pes_start flag */
                if( p_sys->p_pes && p_frame->i_buffer > 4 &&
                    p_frame->p_buffer[0] == 0x00 &&
                    p_frame->p_buffer[1] == 0x00 &&
                    p_frame->p_buffer[2] == 0x01 )
                {
                    ParsePES( p_demux );
                }
                block_ChainAppend( &p_sys->p_pes, p_frame );
            }
            break;
        default:
            msg_Warn( p_demux, "unknown id=0x%x", p_peek[2] );
            stream_Read( p_demux->s, NULL, i_size + 8 );
            break;
    }
    return 1;
}
