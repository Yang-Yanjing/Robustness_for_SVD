 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_KATE )
    {
        return VLC_EGENERIC;
    }
    msg_Dbg( p_dec, "kate: OpenDecoder");
    /* Set callbacks */
    p_dec->pf_decode_sub = (subpicture_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    p_dec->pf_packetize    = (block_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc(sizeof(*p_sys)) ) == NULL )
        return VLC_ENOMEM;
    vlc_mutex_init( &p_sys->lock );
    p_sys->i_refcount = 0;
    DecSysHold( p_sys );
    /* init of p_sys */
#ifdef ENABLE_PACKETIZER
    p_sys->b_packetizer = false;
#endif
    p_sys->b_ready = false;
    p_sys->i_pts =
    p_sys->i_max_stop = VLC_TS_INVALID;
    kate_comment_init( &p_sys->kc );
    kate_info_init( &p_sys->ki );
    p_sys->b_has_headers = false;
    /* retrieve options */
    p_sys->b_formatted = var_CreateGetBool( p_dec, "kate-formatted" );
    vlc_mutex_lock( &kate_decoder_list_mutex );
#ifdef HAVE_TIGER
    p_sys->b_use_tiger = var_CreateGetBool( p_dec, "kate-use-tiger" );
    p_sys->p_tr = NULL;
    /* get initial value of configuration */
    p_sys->i_tiger_default_font_color = GetTigerColor( p_dec, "kate-tiger-default-font" );
    p_sys->i_tiger_default_background_color = GetTigerColor( p_dec, "kate-tiger-default-background" );
    p_sys->e_tiger_default_font_effect = GetTigerInteger( p_dec, "kate-tiger-default-font-effect" );
    p_sys->f_tiger_default_font_effect_strength = GetTigerFloat( p_dec, "kate-tiger-default-font-effect-strength" );
    p_sys->psz_tiger_default_font_desc = GetTigerString( p_dec, "kate-tiger-default-font-desc" );
    p_sys->f_tiger_quality = GetTigerFloat( p_dec, "kate-tiger-quality" );
    if( p_sys->b_use_tiger )
    {
        int i_ret = tiger_renderer_create( &p_sys->p_tr );
        if( i_ret < 0 )
        {
            msg_Warn ( p_dec, "Failed to create Tiger renderer, falling back to basic rendering" );
            p_sys->p_tr = NULL;
            p_sys->b_use_tiger = false;
        }
        else {
            CHECK_TIGER_RET( tiger_renderer_set_surface_clear_color( p_sys->p_tr, 1, 0, 0, 0, 0 ) );
            UpdateTigerFontEffect( p_dec );
            UpdateTigerFontColor( p_dec );
            UpdateTigerBackgroundColor( p_dec );
            UpdateTigerQuality( p_dec );
            UpdateTigerFontDesc( p_dec );
        }
    }
#else
    p_sys->b_use_tiger = false;
#endif
    es_format_Init( &p_dec->fmt_out, SPU_ES, 0 );
    /* add the decoder to the global list */
    decoder_t **list = realloc( kate_decoder_list, (kate_decoder_list_size+1) * sizeof( *list ));
    if( list )
    {
        list[ kate_decoder_list_size++ ] = p_dec;
        kate_decoder_list = list;
    }
    vlc_mutex_unlock( &kate_decoder_list_mutex );
    return VLC_SUCCESS;
}
