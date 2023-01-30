 *****************************************************************************/
static int OpenCommon( vlc_object_t *p_this, bool b_packetizer )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    int i_type;
    int i_header_size;
    switch( p_dec->fmt_in.i_codec )
    {
    /* DVD LPCM */
    case VLC_CODEC_DVD_LPCM:
        i_type = LPCM_VOB;
        i_header_size = LPCM_VOB_HEADER_LEN;
        break;
    /* DVD-Audio LPCM */
    case VLC_CODEC_DVDA_LPCM:
        i_type = LPCM_AOB;
        i_header_size = LPCM_AOB_HEADER_LEN;
        break;
    /* BD LPCM */
    case VLC_CODEC_BD_LPCM:
        i_type = LPCM_BD;
        i_header_size = LPCM_BD_HEADER_LEN;
        break;
    /* WIDI LPCM */
    case VLC_CODEC_WIDI_LPCM:
        i_type = LPCM_WIDI;
        i_header_size = LPCM_WIDI_HEADER_LEN;
        break;
    default:
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc(sizeof(decoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    /* Misc init */
    p_sys->b_packetizer = b_packetizer;
    date_Set( &p_sys->end_date, 0 );
    p_sys->i_type = i_type;
    p_sys->i_header_size = i_header_size;
    p_sys->i_chans_to_reorder = 0;
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    if( b_packetizer )
    {
        switch( i_type )
        {
        case LPCM_VOB:
            p_dec->fmt_out.i_codec = VLC_CODEC_DVD_LPCM;
            break;
        case LPCM_AOB:
            p_dec->fmt_out.i_codec = VLC_CODEC_DVDA_LPCM;
            break;
        case LPCM_WIDI:
            p_dec->fmt_out.i_codec = VLC_CODEC_WIDI_LPCM;
            break;
        default:
            assert(0);
        case LPCM_BD:
            p_dec->fmt_out.i_codec = VLC_CODEC_BD_LPCM;
            break;
        }
    }
    else
    {
        switch( p_dec->fmt_out.audio.i_bitspersample )
        {
        case 24:
        case 20:
            p_dec->fmt_out.i_codec = VLC_CODEC_S32N;
            p_dec->fmt_out.audio.i_bitspersample = 32;
            break;
        default:
            p_dec->fmt_out.i_codec = VLC_CODEC_S16N;
            p_dec->fmt_out.audio.i_bitspersample = 16;
            break;
        }
    }
    /* Set callback */
    p_dec->pf_decode_audio = DecodeFrame;
    p_dec->pf_packetize    = DecodeFrame;
    return VLC_SUCCESS;
}
