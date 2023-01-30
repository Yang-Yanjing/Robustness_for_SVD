 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_MPGV )
        return VLC_EGENERIC;
    es_format_Init( &p_dec->fmt_out, VIDEO_ES, VLC_CODEC_MPGV );
    p_dec->fmt_out.i_original_fourcc = p_dec->fmt_in.i_original_fourcc;
    p_dec->pf_packetize = Packetize;
    p_dec->pf_get_cc = GetCc;
    p_dec->p_sys = p_sys = malloc( sizeof( decoder_sys_t ) );
    if( !p_dec->p_sys )
        return VLC_ENOMEM;
    memset( p_dec->p_sys, 0, sizeof( decoder_sys_t ) );
    /* Misc init */
    packetizer_Init( &p_sys->packetizer,
                     p_mp2v_startcode, sizeof(p_mp2v_startcode),
                     NULL, 0, 4,
                     PacketizeReset, PacketizeParse, PacketizeValidate, p_dec );
    p_sys->p_seq = NULL;
    p_sys->p_ext = NULL;
    p_sys->p_frame = NULL;
    p_sys->pp_last = &p_sys->p_frame;
    p_sys->b_frame_slice = false;
    p_sys->i_dts = p_sys->i_pts = VLC_TS_INVALID;
    p_sys->i_frame_rate = 1;
    p_sys->i_frame_rate_base = 1;
    p_sys->b_seq_progressive = true;
    p_sys->b_low_delay = true;
    p_sys->i_seq_old = 0;
    p_sys->i_temporal_ref = 0;
    p_sys->i_picture_type = 0;
    p_sys->i_picture_structure = 0x03; /* frame */
    p_sys->i_top_field_first = 0;
    p_sys->i_repeat_first_field = 0;
    p_sys->i_progressive_frame = 0;
    p_sys->b_inited = 0;
    p_sys->i_interpolated_dts = VLC_TS_INVALID;
    p_sys->i_last_ref_pts = VLC_TS_INVALID;
    p_sys->b_second_field = 0;
    p_sys->b_discontinuity = false;
    p_sys->b_sync_on_intra_frame = var_CreateGetBool( p_dec, "packetizer-mpegvideo-sync-iframe" );
    if( p_sys->b_sync_on_intra_frame )
        msg_Dbg( p_dec, "syncing on intra frame now" );
    p_sys->b_cc_reset = false;
    p_sys->i_cc_pts = 0;
    p_sys->i_cc_dts = 0;
    p_sys->i_cc_flags = 0;
    cc_Init( &p_sys->cc );
    return VLC_SUCCESS;
}
