 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    int i_field;
    int i_channel;
    switch( p_dec->fmt_in.i_codec )
    {
        case VLC_FOURCC('c','c','1',' '):
            i_field = 0; i_channel = 1;
            break;
        case VLC_FOURCC('c','c','2',' '):
            i_field = 0; i_channel = 2;
            break;
        case VLC_FOURCC('c','c','3',' '):
            i_field = 1; i_channel = 1;
            break;
        case VLC_FOURCC('c','c','4',' '):
            i_field = 1; i_channel = 2;
            break;
        default:
            return VLC_EGENERIC;
    }
    p_dec->pf_decode_sub = Decode;
    /* Allocate the memory needed to store the decoder's structure */
    p_dec->p_sys = p_sys = calloc( 1, sizeof( *p_sys ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    /* init of p_sys */
    p_sys->i_field = i_field;
    p_sys->i_channel = i_channel;
    Eia608Init( &p_sys->eia608 );
    p_dec->fmt_out.i_cat = SPU_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_TEXT;
    return VLC_SUCCESS;
}
