 *****************************************************************************/
static int OpenPostproc( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    vlc_value_t val, val_orig, text;
    int i_flags = 0;
    if( p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma ||
        p_filter->fmt_in.video.i_height != p_filter->fmt_out.video.i_height ||
        p_filter->fmt_in.video.i_width != p_filter->fmt_out.video.i_width )
    {
        msg_Err( p_filter, "Filter input and output formats must be identical" );
        return VLC_EGENERIC;
    }
    /* Set CPU capabilities */
#if defined(__i386__) || defined(__x86_64__)
    if( vlc_CPU_MMX() )
        i_flags |= PP_CPU_CAPS_MMX;
    if( vlc_CPU_MMXEXT() )
        i_flags |= PP_CPU_CAPS_MMX2;
    if( vlc_CPU_3dNOW() )
        i_flags |= PP_CPU_CAPS_3DNOW;
#elif defined(__ppc__) || defined(__ppc64__) || defined(__powerpc__)
    if( vlc_CPU_ALTIVEC() )
        i_flags |= PP_CPU_CAPS_ALTIVEC;
#endif
    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_I444:
        case VLC_CODEC_J444:
        /* case VLC_CODEC_YUVA:
           FIXME: Should work but alpha plane needs to be copied manually and
                  I'm kind of feeling too lazy to write the code to do that ATM
                  (i_pitch vs i_visible_pitch...). */
            i_flags |= PP_FORMAT_444;
            break;
        case VLC_CODEC_I422:
        case VLC_CODEC_J422:
            i_flags |= PP_FORMAT_422;
            break;
        case VLC_CODEC_I411:
            i_flags |= PP_FORMAT_411;
            break;
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
        case VLC_CODEC_YV12:
            i_flags |= PP_FORMAT_420;
            break;
        default:
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                      (char*)&p_filter->fmt_in.video.i_chroma );
            return VLC_EGENERIC;
    }
    p_sys = malloc( sizeof( filter_sys_t ) );
    if( !p_sys ) return VLC_ENOMEM;
    p_filter->p_sys = p_sys;
    p_sys->pp_context = pp_get_context( p_filter->fmt_in.video.i_width,
                                        p_filter->fmt_in.video.i_height,
                                        i_flags );
    if( !p_sys->pp_context )
    {
        msg_Err( p_filter, "Error while creating post processing context." );
        free( p_sys );
        return VLC_EGENERIC;
    }
    config_ChainParse( p_filter, FILTER_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    var_Create( p_filter, FILTER_PREFIX "q", VLC_VAR_INTEGER |
                VLC_VAR_HASCHOICE | VLC_VAR_DOINHERIT | VLC_VAR_ISCOMMAND );
    text.psz_string = _("Post processing");
    var_Change( p_filter, FILTER_PREFIX "q", VLC_VAR_SETTEXT, &text, NULL );
    var_Get( p_filter, FILTER_PREFIX "q", &val_orig );
    var_Change( p_filter, FILTER_PREFIX "q", VLC_VAR_DELCHOICE, &val_orig, NULL );
    val.psz_string = var_GetNonEmptyString( p_filter, FILTER_PREFIX "name" );
    if( val_orig.i_int )
    {
        p_sys->pp_mode = pp_get_mode_by_name_and_quality( val.psz_string ?
                                                          val.psz_string :
                                                          "default",
                                                          val_orig.i_int );
        if( !p_sys->pp_mode )
        {
            msg_Err( p_filter, "Error while creating post processing mode." );
            free( val.psz_string );
            pp_free_context( p_sys->pp_context );
            free( p_sys );
            return VLC_EGENERIC;
        }
    }
    else
    {
        p_sys->pp_mode = NULL;
    }
    free( val.psz_string );
    for( val.i_int = 0; val.i_int <= PP_QUALITY_MAX; val.i_int++ )
    {
        switch( val.i_int )
        {
            case 0:
                text.psz_string = _("Disable");
                break;
            case 1:
                text.psz_string = _("Lowest");
                break;
            case PP_QUALITY_MAX:
                text.psz_string = _("Highest");
                break;
            default:
                text.psz_string = NULL;
                break;
        }
        var_Change( p_filter, FILTER_PREFIX "q", VLC_VAR_ADDCHOICE,
                    &val, text.psz_string?&text:NULL );
    }
    vlc_mutex_init( &p_sys->lock );
    /* Add the callback at the end to prevent crashes */
    var_AddCallback( p_filter, FILTER_PREFIX "q", PPQCallback, NULL );
    var_AddCallback( p_filter, FILTER_PREFIX "name", PPNameCallback, NULL );
    p_filter->pf_video_filter = PostprocPict;
    msg_Warn( p_filter, "Quantification table was not set by video decoder. "
                        "Postprocessing won't look good." );
    return VLC_SUCCESS;
}
