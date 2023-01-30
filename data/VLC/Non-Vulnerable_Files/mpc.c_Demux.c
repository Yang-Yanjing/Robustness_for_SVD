 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_data;
    int i_ret;
#ifdef HAVE_MPC_MPCDEC_H
    mpc_frame_info frame;
    mpc_status err;
#endif
    p_data = block_Alloc( MPC_DECODER_BUFFER_LENGTH*sizeof(MPC_SAMPLE_FORMAT) );
    if( !p_data )
        return -1;
#ifndef HAVE_MPC_MPCDEC_H
    i_ret = mpc_decoder_decode( &p_sys->decoder,
                                (MPC_SAMPLE_FORMAT*)p_data->p_buffer,
                                NULL, NULL );
    if( i_ret <= 0 )
    {
        block_Release( p_data );
        return i_ret < 0 ? -1 : 0;
    }
#else
    frame.buffer = (MPC_SAMPLE_FORMAT*)p_data->p_buffer;
    err = mpc_demux_decode( p_sys->decoder, &frame );
    if( err != MPC_STATUS_OK )
    {
        block_Release( p_data );
        return -1;
    }
    else if( frame.bits == -1 )
    {
        block_Release( p_data );
        return 0;
    }
    i_ret = frame.samples;
#endif
    /* */
    p_data->i_buffer = i_ret * sizeof(MPC_SAMPLE_FORMAT) * p_sys->info.channels;
    p_data->i_dts = p_data->i_pts =
            VLC_TS_0 + INT64_C(1000000) * p_sys->i_position / p_sys->info.sample_freq;
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_data->i_dts );
    es_out_Send( p_demux->out, p_sys->p_es, p_data );
    /* */
    p_sys->i_position += i_ret;
    return 1;
}
