 ****************************************************************************/
static int Open( decoder_t *p_dec, bool b_packetizer )
{
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_302M )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the decoder's structure */
    p_dec->p_sys = p_sys = malloc( sizeof(decoder_sys_t) );
    if( unlikely( !p_sys ) )
        return VLC_EGENERIC;
    /* Misc init */
    date_Init( &p_sys->end_date, 48000, 1 );
    date_Set( &p_sys->end_date, 0 );
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.audio.i_rate = 48000;
    /* Set callback */
    if( b_packetizer )
    {
        p_dec->fmt_out.i_codec = VLC_CODEC_302M;
        p_dec->pf_decode_audio = NULL;
        p_dec->pf_packetize    = Packetize;
    }
    else
    {
        p_dec->fmt_out.i_codec = VLC_CODEC_S16N;
        p_dec->fmt_out.audio.i_bitspersample = 16;
        p_dec->pf_decode_audio = Decode;
        p_dec->pf_packetize    = NULL;
    }
    return VLC_SUCCESS;
}
