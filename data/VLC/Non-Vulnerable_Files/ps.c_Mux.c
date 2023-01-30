 *****************************************************************************/
static int Mux( sout_mux_t *p_mux )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    for( ;; )
    {
        sout_input_t *p_input;
        ps_stream_t *p_stream;
        block_t *p_ps, *p_data;
        mtime_t        i_dts;
        /* Choose which stream to mux */
        int i_stream = sout_MuxGetStream( p_mux, 1, &i_dts );
        if( i_stream < 0 )
        {
            return VLC_SUCCESS;
        }
        p_input  = p_mux->pp_inputs[i_stream];
        p_stream = (ps_stream_t*)p_input->p_sys;
        p_ps     = NULL;
        /* Write regulary PackHeader */
        if( p_sys->i_pes_count % 30 == 0)
        {
            /* Update the instant bitrate every second or so */
            if( p_sys->i_instant_size &&
                i_dts - p_sys->i_instant_dts > 1000000 )
            {
                int64_t i_instant_bitrate = p_sys->i_instant_size * 8000000 /
                    ( i_dts - p_sys->i_instant_dts );
                p_sys->i_instant_bitrate += i_instant_bitrate;
                p_sys->i_instant_bitrate /= 2;
                p_sys->i_instant_size = 0;
                p_sys->i_instant_dts = i_dts;
            }
            else if( !p_sys->i_instant_size )
            {
                p_sys->i_instant_dts = i_dts;
            }
            MuxWritePackHeader( p_mux, &p_ps, i_dts );
        }
        /* Write regulary SystemHeader */
        if( p_sys->i_pes_count % 300 == 0 )
        {
            block_t *p_pk;
            MuxWriteSystemHeader( p_mux, &p_ps, i_dts );
            /* For MPEG1 streaming, set HEADER flag */
            for( p_pk = p_ps; p_pk != NULL; p_pk = p_pk->p_next )
            {
                p_pk->i_flags |= BLOCK_FLAG_HEADER;
            }
        }
        /* Write regulary ProgramStreamMap */
        if( p_sys->b_mpeg2 && p_sys->i_pes_count % 300 == 0 )
        {
            MuxWritePSM( p_mux, &p_ps, i_dts );
        }
        /* Get and mux a packet */
        p_data = block_FifoGet( p_input->p_fifo );
         EStoPES ( &p_data, p_data, p_input->p_fmt, p_stream->i_stream_id,
                       p_sys->b_mpeg2, 0, 0, p_sys->i_pes_max_size );
        block_ChainAppend( &p_ps, p_data );
        /* Get size of output data so we can calculate the instant bitrate */
        for( p_data = p_ps; p_data; p_data = p_data->p_next )
        {
            p_sys->i_instant_size += p_data->i_buffer;
        }
        sout_AccessOutWrite( p_mux->p_access, p_ps );
        /* Increase counter */
        p_sys->i_pes_count++;
    }
    return VLC_SUCCESS;
}
