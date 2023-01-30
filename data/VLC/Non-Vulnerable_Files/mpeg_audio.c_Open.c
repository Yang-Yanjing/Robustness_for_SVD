 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if(( p_dec->fmt_in.i_codec != VLC_CODEC_MPGA ) &&
       ( p_dec->fmt_in.i_codec != VLC_CODEC_MP3 ) )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys =
          (decoder_sys_t *)malloc(sizeof(decoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    /* Misc init */
    p_sys->b_packetizer = false;
    p_sys->i_state = STATE_NOSYNC;
    date_Set( &p_sys->end_date, 0 );
    block_BytestreamInit( &p_sys->bytestream );
    p_sys->i_pts = VLC_TS_INVALID;
    p_sys->b_discontinuity = false;
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_MPGA;
    p_dec->fmt_out.audio.i_rate = 0; /* So end_date gets initialized */
    /* Set callback */
    p_dec->pf_decode_audio = DecodeBlock;
    p_dec->pf_packetize    = DecodeBlock;
    /* Start with the minimum size for a free bitrate frame */
    p_sys->i_free_frame_size = MPGA_HEADER_SIZE;
    return VLC_SUCCESS;
}
