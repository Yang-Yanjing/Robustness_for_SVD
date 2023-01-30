 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    const uint8_t *p_peek, *p_peek_backup;
    uint32_t    i_dword;
    dv_header_t dv_header;
    dv_id_t     dv_id;
    /* It isn't easy to recognize a raw DV stream. The chances that we'll
     * mistake a stream from another type for a raw DV stream are too high, so
     * we'll rely on the file extension to trigger this demux. Alternatively,
     * it is possible to force this demux. */
    /* Check for DV file extension */
    if( !demux_IsPathExtension( p_demux, ".dv" ) && !p_demux->b_force )
        return VLC_EGENERIC;
    if( stream_Peek( p_demux->s, &p_peek, DV_PAL_FRAME_SIZE ) <
        DV_NTSC_FRAME_SIZE )
    {
        /* Stream too short ... */
        msg_Err( p_demux, "cannot peek()" );
        return VLC_EGENERIC;
    }
    p_peek_backup = p_peek;
    /* fill in the dv_id_t structure */
    i_dword = GetDWBE( p_peek ); p_peek += 4;
    dv_id.sct = i_dword >> (32 - 3);
    i_dword <<= 8;
    dv_id.dsn = i_dword >> (32 - 4);
    i_dword <<= 4;
    dv_id.fsc = i_dword >> (32 - 1);
    i_dword <<= 4;
    dv_id.dbn = i_dword >> (32 - 8);
    i_dword <<= 8;
    if( dv_id.sct != 0 )
    {
        msg_Warn( p_demux, "not a raw DV stream header" );
        return VLC_EGENERIC;
    }
    /* fill in the dv_header_t structure */
    dv_header.dsf = i_dword >> (32 - 1);
    i_dword <<= 1;
    if( i_dword >> (32 - 1) ) /* incorrect bit */
    {
        msg_Warn( p_demux, "incorrect bit" );
        return VLC_EGENERIC;
    }
    i_dword = GetDWBE( p_peek ); p_peek += 4;
    i_dword <<= 5;
    dv_header.apt = i_dword >> (32 - 3);
    i_dword <<= 3;
    dv_header.tf1 = i_dword >> (32 - 1);
    i_dword <<= 5;
    dv_header.ap1 = i_dword >> (32 - 3);
    i_dword <<= 3;
    dv_header.tf2 = i_dword >> (32 - 1);
    i_dword <<= 5;
    dv_header.ap2 = i_dword >> (32 - 3);
    i_dword <<= 3;
    dv_header.tf3 = i_dword >> (32 - 1);
    i_dword <<= 5;
    dv_header.ap3 = i_dword >> (32 - 3);
    p_peek += 72;                                  /* skip rest of DIF block */
    p_demux->p_sys      = p_sys = malloc( sizeof( demux_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->b_hurry_up = var_CreateGetBool( p_demux, "rawdv-hurry-up" );
    msg_Dbg( p_demux, "Realtime DV Source: %s", (p_sys->b_hurry_up)?"Yes":"No" );
    p_sys->i_dsf = dv_header.dsf;
    p_sys->frame_size = dv_header.dsf ? DV_PAL_FRAME_SIZE
                                      : DV_NTSC_FRAME_SIZE;
    p_sys->f_rate = dv_header.dsf ? 25 : 29.97;
    p_sys->i_pcr = 0;
    p_sys->p_es_video = NULL;
    p_sys->p_es_audio = NULL;
    p_sys->i_bitrate = 0;
    es_format_Init( &p_sys->fmt_video, VIDEO_ES, VLC_CODEC_DV );
    p_sys->fmt_video.video.i_width = 720;
    p_sys->fmt_video.video.i_height= dv_header.dsf ? 576 : 480;;
    p_sys->p_es_video = es_out_Add( p_demux->out, &p_sys->fmt_video );
    /* Audio stuff */
    p_peek = p_peek_backup + 80*6+80*16*3 + 3; /* beginning of AAUX pack */
    if( *p_peek == 0x50 )
    {
        dv_get_audio_format( &p_sys->fmt_audio, &p_peek[1] );
        p_sys->p_es_audio = es_out_Add( p_demux->out, &p_sys->fmt_audio );
    }
    p_demux->pf_demux   = Demux;
    p_demux->pf_control = Control;
    return VLC_SUCCESS;
}
