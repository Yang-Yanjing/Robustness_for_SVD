}
static int Create( vlc_object_t *p_this )
{
    filter_t      *p_filter = (filter_t *)p_this;
    filter_sys_t  *p_sys;
    char          *psz_fontfile   = NULL;
    char          *psz_fontname = NULL;
    char          *psz_monofontfile   = NULL;
    char          *psz_monofontfamily = NULL;
    int            fontindex = 0, monofontindex = 0;
    /* Allocate structure */
    p_filter->p_sys = p_sys = malloc( sizeof(*p_sys) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->style.psz_fontname   = NULL;
    p_sys->p_xml            = NULL;
    p_sys->p_face           = 0;
    p_sys->p_library        = 0;
    p_sys->style.i_font_size      = 0;
    p_sys->style.i_style_flags = 0;
    /*
     * The following variables should not be cached, as they might be changed on-the-fly:
     * freetype-rel-fontsize, freetype-background-opacity, freetype-background-color,
     * freetype-outline-thickness, freetype-color
     *
     */
    psz_fontname = var_InheritString( p_filter, "freetype-font" );
    psz_monofontfamily = var_InheritString( p_filter, "freetype-monofont" );
    p_sys->i_default_font_size = var_InheritInteger( p_filter, "freetype-fontsize" );
    p_sys->style.i_font_alpha = var_InheritInteger( p_filter,"freetype-opacity" );
    p_sys->style.i_font_alpha = VLC_CLIP( p_sys->style.i_font_alpha, 0, 255 );
    if( var_InheritBool( p_filter, "freetype-bold" ) )
        p_sys->style.i_style_flags |= STYLE_BOLD;
    double f_outline_thickness = var_InheritInteger( p_filter, "freetype-outline-thickness" ) / 100.0;
    f_outline_thickness = VLC_CLIP( f_outline_thickness, 0.0, 0.5 );
    p_sys->style.i_outline_alpha = var_InheritInteger( p_filter, "freetype-outline-opacity" );
    p_sys->style.i_outline_alpha = VLC_CLIP( p_sys->style.i_outline_alpha, 0, 255 );
    p_sys->style.i_outline_color = var_InheritInteger( p_filter, "freetype-outline-color" );
    p_sys->style.i_outline_color = VLC_CLIP( p_sys->style.i_outline_color, 0, 0xFFFFFF );
    p_sys->style.i_shadow_alpha = var_InheritInteger( p_filter, "freetype-shadow-opacity" );
    p_sys->style.i_shadow_alpha = VLC_CLIP( p_sys->style.i_shadow_alpha, 0, 255 );
    p_sys->style.i_shadow_color = var_InheritInteger( p_filter, "freetype-shadow-color" );
    p_sys->style.i_shadow_color = VLC_CLIP( p_sys->style.i_shadow_color, 0, 0xFFFFFF );
    float f_shadow_angle = var_InheritFloat( p_filter, "freetype-shadow-angle" );
    float f_shadow_distance = var_InheritFloat( p_filter, "freetype-shadow-distance" );
    f_shadow_distance = VLC_CLIP( f_shadow_distance, 0, 1 );
    p_sys->f_shadow_vector_x = f_shadow_distance * cos(2 * M_PI * f_shadow_angle / 360);
    p_sys->f_shadow_vector_y = f_shadow_distance * sin(2 * M_PI * f_shadow_angle / 360);
    /* Set default psz_fontname */
    if( !psz_fontname || !*psz_fontname )
    {
        free( psz_fontname );
#ifdef HAVE_GET_FONT_BY_FAMILY_NAME
        psz_fontname = strdup( DEFAULT_FAMILY );
#else
        psz_fontname = File_Select( DEFAULT_FONT_FILE );
#endif
    }
    /* set default psz_monofontname */
    if( !psz_monofontfamily || !*psz_monofontfamily )
    {
        free( psz_monofontfamily );
#ifdef HAVE_GET_FONT_BY_FAMILY_NAME
        psz_monofontfamily = strdup( DEFAULT_MONOSPACE_FAMILY );
#else
        psz_monofontfamily = File_Select( DEFAULT_MONOSPACE_FONT_FILE );
#endif
    }
    /* Set the current font file */
    p_sys->style.psz_fontname = psz_fontname;
    p_sys->style.psz_monofontname = psz_monofontfamily;
#ifdef HAVE_FONTCONFIG
    p_sys->pf_select = FontConfig_Select;
    FontConfig_BuildCache( p_filter );
#elif defined( __APPLE__ )
#if !TARGET_OS_IPHONE
    p_sys->pf_select = MacLegacy_Select;
#endif
#elif defined( _WIN32 ) && defined( HAVE_GET_FONT_BY_FAMILY_NAME )
    p_sys->pf_select = Win32_Select;
#else
    p_sys->pf_select = Dummy_Select;
#endif
    /* */
    psz_fontfile = p_sys->pf_select( p_filter, psz_fontname, false, false,
                                      p_sys->i_default_font_size, &fontindex );
    psz_monofontfile = p_sys->pf_select( p_filter, psz_monofontfamily, false,
                                          false, p_sys->i_default_font_size,
                                          &monofontindex );
    msg_Dbg( p_filter, "Using %s as font from file %s", psz_fontname, psz_fontfile );
    msg_Dbg( p_filter, "Using %s as mono-font from file %s", psz_monofontfamily, psz_monofontfile );
    /* If nothing is found, use the default family */
    if( !psz_fontfile )
        psz_fontfile = File_Select( psz_fontname );
    if( !psz_monofontfile )
        psz_monofontfile = File_Select( psz_monofontfamily );
    if( Init_FT( p_this, psz_fontfile, fontindex, f_outline_thickness ) != VLC_SUCCESS )
        goto error;
    p_sys->pp_font_attachments = NULL;
    p_sys->i_font_attachments = 0;
    p_filter->pf_render_text = RenderText;
    p_filter->pf_render_html = RenderHtml;
    LoadFontsFromAttachments( p_filter );
    free( psz_fontfile );
    free( psz_monofontfile );
    return VLC_SUCCESS;
error:
    free( psz_fontfile );
    free( psz_monofontfile );
    free( psz_fontname );
    free( psz_monofontfamily );
    free( p_sys );
    return VLC_EGENERIC;
}
