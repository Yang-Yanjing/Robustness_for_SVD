 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    frame_header_t fh;
    block_t *p_data;
    for( ;; )
    {
        if( !vlc_object_alive (p_demux) )
            return -1;
        if( FrameHeaderLoad( p_demux, &fh ) )
            return 0;
        if( fh.i_type == 'A' || fh.i_type == 'V' )
            break;
        /* TODO add support for some block type */
        if( fh.i_type != 'R' && fh.i_length > 0 )
        {
            if( stream_Read( p_demux->s, NULL, fh.i_length ) != fh.i_length )
                return -1;
        }
    }
    /* */
    if( ( p_data = stream_Block( p_demux->s, fh.i_length ) ) == NULL )
        return 0;
    p_data->i_dts = VLC_TS_0 + (int64_t)fh.i_timecode * 1000;
    p_data->i_pts = (fh.i_type == 'V') ? VLC_TS_INVALID : p_data->i_dts;
    /* only add keyframes to index */
    if( !fh.i_keyframe && !p_sys->b_index )
        demux_IndexAppend( &p_sys->idx,
                           p_data->i_dts - VLC_TS_0,
                           stream_Tell(p_demux->s) - NUV_FH_SIZE );
    /* */
    if( p_sys->i_pcr < 0 || p_sys->i_pcr < p_data->i_dts - VLC_TS_0 )
    {
        p_sys->i_pcr = p_data->i_dts - VLC_TS_0;
        es_out_Control( p_demux->out, ES_OUT_SET_PCR, VLC_TS_0 + p_sys->i_pcr );
    }
    if( fh.i_type == 'A' && p_sys->p_es_audio )
    {
        if( fh.i_compression == '3' )
            es_out_Send( p_demux->out, p_sys->p_es_audio, p_data );
        else
        {
            msg_Dbg( p_demux, "unsupported compression %c for audio (upload samples)", fh.i_compression );
            block_Release( p_data );
        }
    }
    else if( fh.i_type == 'V' && p_sys->p_es_video )
    {
        if( fh.i_compression >='0' && fh.i_compression <='3' )
        {
            /* for rtjpeg data, the header is also needed */
            p_data = block_Realloc( p_data, NUV_FH_SIZE, fh.i_length );
            if( unlikely(!p_data) )
                abort();
            memcpy( p_data->p_buffer, p_sys->fh_buffer, NUV_FH_SIZE );
        }
        /* 0,1,2,3 -> rtjpeg, >=4 mpeg4 */
        if( fh.i_compression >= '0' )
            es_out_Send( p_demux->out, p_sys->p_es_video, p_data );
        else
        {
            msg_Dbg( p_demux, "unsupported compression %c for video (upload samples)", fh.i_compression );
            block_Release( p_data );
        }
    }
    else
    {
        block_Release( p_data );
    }
    return 1;
}
