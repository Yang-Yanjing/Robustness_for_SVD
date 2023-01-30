 *****************************************************************************/
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys;
    if( ( p_enc->fmt_out.i_codec != VLC_CODEC_DVBS ) &&
        !p_enc->b_force )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_sys = (encoder_sys_t *)malloc(sizeof(encoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    p_enc->p_sys = p_sys;
    p_enc->pf_encode_sub = Encode;
    p_enc->fmt_out.i_codec = VLC_CODEC_DVBS;
    p_enc->fmt_out.subs.dvb.i_id  = 1 << 16 | 1;
    config_ChainParse( p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg );
    p_sys->i_page_ver = 0;
    p_sys->i_region_ver = 0;
    p_sys->i_clut_ver = 0;
    p_sys->i_regions = 0;
    p_sys->p_regions = 0;
    p_sys->i_offset_x = var_CreateGetInteger( p_this, ENC_CFG_PREFIX "x" );
    p_sys->i_offset_y = var_CreateGetInteger( p_this, ENC_CFG_PREFIX "y" );
    return VLC_SUCCESS;
}
