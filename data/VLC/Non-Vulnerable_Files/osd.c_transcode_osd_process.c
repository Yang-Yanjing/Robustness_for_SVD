}
int transcode_osd_process( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                                  block_t *in, block_t **out )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    subpicture_t *p_subpic = NULL;
    /* Check if we have a subpicture to send */
    if( p_sys->p_spu && in->i_dts > VLC_TS_INVALID )
    {
        video_format_t fmt;
        video_format_Init( &fmt, 0 );
        video_format_Setup( &fmt, 0, 720, 576, 720, 576, 1, 1 );
        p_subpic = spu_Render( p_sys->p_spu, NULL, &fmt, &fmt, in->i_dts, in->i_dts, false );
    }
    else
    {
        msg_Warn( p_stream, "spu channel not initialized, doing it now" );
        if( !p_sys->p_spu )
            p_sys->p_spu = spu_Create( p_stream );
    }
    if( p_subpic )
    {
        block_t *p_block = NULL;
        if( p_sys->b_master_sync && p_sys->i_master_drift )
        {
            p_subpic->i_start -= p_sys->i_master_drift;
            if( p_subpic->i_stop ) p_subpic->i_stop -= p_sys->i_master_drift;
        }
        p_block = id->p_encoder->pf_encode_sub( id->p_encoder, p_subpic );
        subpicture_Delete( p_subpic );
        if( p_block )
        {
            p_block->i_dts = p_block->i_pts = in->i_dts;
            block_ChainAppend( out, p_block );
            return VLC_SUCCESS;
        }
    }
    return VLC_EGENERIC;
}
