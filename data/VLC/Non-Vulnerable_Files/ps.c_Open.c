 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_mux_t *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t *p_sys;
    vlc_value_t val;
    msg_Info( p_mux, "Open" );
    config_ChainParse( p_mux, SOUT_CFG_PREFIX, ppsz_sout_options, p_mux->p_cfg );
    p_mux->pf_control   = Control;
    p_mux->pf_addstream = AddStream;
    p_mux->pf_delstream = DelStream;
    p_mux->pf_mux       = Mux;
    p_mux->p_sys        = p_sys = malloc( sizeof( sout_mux_sys_t ) );
    /* Init free stream id */
    StreamIdInit( p_sys->stream_id_a52,  8  );
    StreamIdInit( p_sys->stream_id_dts,  8  );
    StreamIdInit( p_sys->stream_id_mpga, 16 );
    StreamIdInit( p_sys->stream_id_mpgv, 16 );
    StreamIdInit( p_sys->stream_id_lpcm, 16 );
    StreamIdInit( p_sys->stream_id_spu,  32 );
    p_sys->i_audio_bound   = 0;
    p_sys->i_video_bound   = 0;
    p_sys->i_system_header = 0;
    p_sys->i_pes_count     = 0;
    p_sys->i_psm_version   = 0;
    p_sys->i_instant_bitrate  = 0;
    p_sys->i_instant_size     = 0;
    p_sys->i_instant_dts      = 0;
    p_sys->i_rate_bound      = 0;
    p_sys->b_mpeg2 = !(p_mux->psz_mux && !strcmp( p_mux->psz_mux, "mpeg1" ));
    var_Get( p_mux, SOUT_CFG_PREFIX "dts-delay", &val );
    p_sys->i_dts_delay = (int64_t)val.i_int * 1000;
    var_Get( p_mux, SOUT_CFG_PREFIX "pes-max-size", &val );
    p_sys->i_pes_max_size = (int64_t)val.i_int;
    /* Initialise CRC32 table */
    if( p_sys->b_mpeg2 )
    {
        uint32_t i, j, k;
        for( i = 0; i < 256; i++ )
        {
            k = 0;
            for( j = (i << 24) | 0x800000; j != 0x80000000; j <<= 1 )
                k = (k << 1) ^ (((k ^ j) & 0x80000000) ? 0x04c11db7 : 0);
            p_sys->crc32_table[i] = k;
        }
    }
    return VLC_SUCCESS;
}
