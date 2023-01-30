 *****************************************************************************/
static int Init_FT( vlc_object_t *p_this,
                    const char *psz_fontfile,
                    const int fontindex,
                    const float f_outline_thickness)
{
    filter_t      *p_filter = (filter_t *)p_this;
    filter_sys_t  *p_sys = p_filter->p_sys;
    /* */
    int i_error = FT_Init_FreeType( &p_sys->p_library );
    if( i_error )
    {
        msg_Err( p_filter, "couldn't initialize freetype" );
        goto error;
    }
    i_error = FT_New_Face( p_sys->p_library, psz_fontfile ? psz_fontfile : "",
                           fontindex, &p_sys->p_face );
    if( i_error == FT_Err_Unknown_File_Format )
    {
        msg_Err( p_filter, "file %s have unknown format",
                 psz_fontfile ? psz_fontfile : "(null)" );
        goto error;
    }
    else if( i_error )
    {
        msg_Err( p_filter, "failed to load font file %s",
                 psz_fontfile ? psz_fontfile : "(null)" );
        goto error;
    }
    i_error = FT_Select_Charmap( p_sys->p_face, ft_encoding_unicode );
    if( i_error )
    {
        msg_Err( p_filter, "font has no unicode translation table" );
        goto error;
    }
    if( SetFontSize( p_filter, 0 ) != VLC_SUCCESS ) goto error;
    p_sys->p_stroker = NULL;
    if( f_outline_thickness > 0.001 )
    {
        i_error = FT_Stroker_New( p_sys->p_library, &p_sys->p_stroker );
        if( i_error )
            msg_Err( p_filter, "Failed to create stroker for outlining" );
    }
    return VLC_SUCCESS;
error:
    if( p_sys->p_face ) FT_Done_Face( p_sys->p_face );
    if( p_sys->p_library ) FT_Done_FreeType( p_sys->p_library );
    return VLC_EGENERIC;
}
