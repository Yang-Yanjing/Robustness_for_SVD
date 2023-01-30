 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_SSA )
        return VLC_EGENERIC;
    p_dec->pf_decode_sub = DecodeBlock;
    p_dec->p_sys = p_sys = malloc( sizeof( decoder_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    /* */
    vlc_mutex_init( &p_sys->lock );
    p_sys->i_refcount = 1;
    memset( &p_sys->fmt, 0, sizeof(p_sys->fmt) );
    p_sys->i_max_stop = VLC_TS_INVALID;
    p_sys->p_library  = NULL;
    p_sys->p_renderer = NULL;
    p_sys->p_track    = NULL;
    /* Create libass library */
    ASS_Library *p_library = p_sys->p_library = ass_library_init();
    if( !p_library )
    {
        msg_Warn( p_dec, "Libass library creation failed" );
        DecSysRelease( p_sys );
        return VLC_EGENERIC;
    }
    /* load attachments */
    input_attachment_t  **pp_attachments;
    int                   i_attachments;
    if( decoder_GetInputAttachments( p_dec, &pp_attachments, &i_attachments ))
    {
        i_attachments = 0;
        pp_attachments = NULL;
    }
    for( int k = 0; k < i_attachments; k++ )
    {
        input_attachment_t *p_attach = pp_attachments[k];
        bool found = false;
        /* Check mimetype*/
        if( !strcasecmp( p_attach->psz_mime, "application/x-truetype-font" ) )
            found = true;
        /* Then extension */
        else if( !found && strlen( p_attach->psz_name ) > 4 )
        {
            char *ext = p_attach->psz_name + strlen( p_attach->psz_name ) - 4;
            if( !strcasecmp( ext, ".ttf" ) || !strcasecmp( ext, ".otf" ) || !strcasecmp( ext, ".ttc" ) )
                found = true;
        }
        if( found )
        {
            msg_Dbg( p_dec, "adding embedded font %s", p_attach->psz_name );
            ass_add_font( p_sys->p_library, p_attach->psz_name, p_attach->p_data, p_attach->i_data );
        }
        vlc_input_attachment_Delete( p_attach );
    }
    free( pp_attachments );
    ass_set_extract_fonts( p_library, true );
    ass_set_style_overrides( p_library, NULL );
    /* Create the renderer */
    ASS_Renderer *p_renderer = p_sys->p_renderer = ass_renderer_init( p_library );
    if( !p_renderer )
    {
        msg_Warn( p_dec, "Libass renderer creation failed" );
        DecSysRelease( p_sys );
        return VLC_EGENERIC;
    }
    ass_set_use_margins( p_renderer, false);
    //if( false )
    //    ass_set_margins( p_renderer, int t, int b, int l, int r);
    ass_set_hinting( p_renderer, ASS_HINTING_LIGHT );
    ass_set_font_scale( p_renderer, 1.0 );
    ass_set_line_spacing( p_renderer, 0.0 );
#if defined( __ANDROID__ )
    const char *psz_font = "/system/fonts/DroidSans-Bold.ttf";
    const char *psz_family = "Droid Sans Bold";
#else
    const char *psz_font = NULL; /* We don't ship a default font with VLC */
    const char *psz_family = "Arial"; /* Use Arial if we can't find anything more suitable */
#endif
#ifdef HAVE_FONTCONFIG
#if defined(_WIN32) || defined(__APPLE__)
    dialog_progress_bar_t *p_dialog =
        dialog_ProgressCreate( p_dec,
                               _("Building font cache"),
                               _( "Please wait while your font cache is rebuilt.\n"
                                  "This should take less than a minute." ), NULL );
#endif
    ass_set_fonts( p_renderer, psz_font, psz_family, 1, NULL, 1 );  // setup default font/family
#if defined(_WIN32) || defined(__APPLE__)
    if( p_dialog )
    {
        dialog_ProgressSet( p_dialog, NULL, 1.0 );
        dialog_ProgressDestroy( p_dialog );
    }
#endif
#else
    /* FIXME you HAVE to give him a font if no fontconfig */
    ass_set_fonts( p_renderer, psz_font, psz_family, 1, NULL, 1 );
#endif
    /* Add a track */
    ASS_Track *p_track = p_sys->p_track = ass_new_track( p_sys->p_library );
    if( !p_track )
    {
        DecSysRelease( p_sys );
        return VLC_EGENERIC;
    }
    ass_process_codec_private( p_track, p_dec->fmt_in.p_extra, p_dec->fmt_in.i_extra );
    p_dec->fmt_out.i_cat = SPU_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_RGBA;
    return VLC_SUCCESS;
}
