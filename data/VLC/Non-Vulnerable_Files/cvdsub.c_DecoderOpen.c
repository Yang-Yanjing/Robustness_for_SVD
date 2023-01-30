 *****************************************************************************/
static int DecoderOpen( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_CVD )
        return VLC_EGENERIC;
    p_dec->p_sys = p_sys = malloc( sizeof( decoder_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->b_packetizer  = false;
    p_sys->i_state = SUBTITLE_BLOCK_EMPTY;
    p_sys->p_spu   = NULL;
    p_dec->pf_decode_sub = Decode;
    p_dec->pf_packetize  = Packetize;
    p_dec->fmt_out.i_cat = SPU_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_YUVP;
    return VLC_SUCCESS;
}
