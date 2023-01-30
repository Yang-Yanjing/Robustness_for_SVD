 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t *) p_this;
    decoder_sys_t *p_sys;
    int i_posx, i_posy;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_DVBS )
    {
        return VLC_EGENERIC;
    }
    p_dec->pf_decode_sub = Decode;
    p_sys = p_dec->p_sys = calloc( 1, sizeof(decoder_sys_t) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_pts          = VLC_TS_INVALID;
    p_sys->i_id           = p_dec->fmt_in.subs.dvb.i_id & 0xFFFF;
    p_sys->i_ancillary_id = p_dec->fmt_in.subs.dvb.i_id >> 16;
    p_sys->p_regions      = NULL;
    p_sys->p_cluts        = NULL;
    p_sys->p_page         = NULL;
    /* configure for SD res in case DDS is not present */
    default_dds_init( p_dec );
    p_sys->i_spu_position = var_CreateGetInteger( p_this,
                                    DVBSUB_CFG_PREFIX "position" );
    i_posx = var_CreateGetInteger( p_this, DVBSUB_CFG_PREFIX "x" );
    i_posy = var_CreateGetInteger( p_this, DVBSUB_CFG_PREFIX "y" );
    /* Check if subpicture position was overridden */
    p_sys->b_absolute = true;
    p_sys->i_spu_x = p_sys->i_spu_y = 0;
    if( ( i_posx >= 0 ) && ( i_posy >= 0 ) )
    {
        p_sys->b_absolute = true;
        p_sys->i_spu_x = i_posx;
        p_sys->i_spu_y = i_posy;
    }
    p_dec->fmt_out.i_cat = SPU_ES;
    p_dec->fmt_out.i_codec = 0;
    default_clut_init( p_dec );
    return VLC_SUCCESS;
}
