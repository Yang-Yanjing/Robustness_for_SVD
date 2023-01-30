 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_MP4V )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc( sizeof(decoder_sys_t) ) ) == NULL )
        return VLC_ENOMEM;
    memset( p_sys, 0, sizeof(decoder_sys_t) );
    /* Misc init */
    packetizer_Init( &p_sys->packetizer,
                     p_mp4v_startcode, sizeof(p_mp4v_startcode),
                     NULL, 0, 4,
                     PacketizeReset, PacketizeParse, PacketizeValidate, p_dec );
    p_sys->p_frame = NULL;
    p_sys->pp_last = &p_sys->p_frame;
    /* Setup properties */
    es_format_Copy( &p_dec->fmt_out, &p_dec->fmt_in );
    p_dec->fmt_out.i_codec = VLC_CODEC_MP4V;
    free(p_dec->fmt_out.p_extra);
    if( p_dec->fmt_in.i_extra )
    {
        /* We have a vol */
        p_dec->fmt_out.i_extra = p_dec->fmt_in.i_extra;
        p_dec->fmt_out.p_extra = xmalloc( p_dec->fmt_in.i_extra );
        memcpy( p_dec->fmt_out.p_extra, p_dec->fmt_in.p_extra,
                p_dec->fmt_in.i_extra );
        msg_Dbg( p_dec, "opening with vol size: %d", p_dec->fmt_in.i_extra );
        ParseVOL( p_dec, &p_dec->fmt_out,
                  p_dec->fmt_out.p_extra, p_dec->fmt_out.i_extra );
    }
    else
    {
        /* No vol, we'll have to look for one later on */
        p_dec->fmt_out.i_extra = 0;
        p_dec->fmt_out.p_extra = 0;
    }
    /* Set callback */
    p_dec->pf_packetize = Packetize;
    return VLC_SUCCESS;
}
