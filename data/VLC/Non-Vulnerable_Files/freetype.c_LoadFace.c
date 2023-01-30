}
static FT_Face LoadFace( filter_t *p_filter,
                         const text_style_t *p_style )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    /* Look for a match amongst our attachments first */
    FT_Face p_face = LoadEmbeddedFace( p_sys, p_style );
    /* Load system wide font otheriwse */
    if( !p_face )
    {
        int  i_idx = 0;
        char *psz_fontfile = NULL;
        if( p_sys->pf_select )
            psz_fontfile = p_sys->pf_select( p_filter,
                                             p_style->psz_fontname,
                                             (p_style->i_style_flags & STYLE_BOLD) != 0,
                                             (p_style->i_style_flags & STYLE_ITALIC) != 0,
                                             -1,
                                             &i_idx );
        else
            psz_fontfile = NULL;
        if( !psz_fontfile )
            return NULL;
        if( *psz_fontfile == '\0' )
        {
            msg_Warn( p_filter,
                      "We were not able to find a matching font: \"%s\" (%s %s),"
                      " so using default font",
                      p_style->psz_fontname,
                      (p_style->i_style_flags & STYLE_BOLD)   ? "Bold" : "",
                      (p_style->i_style_flags & STYLE_ITALIC) ? "Italic" : "" );
            p_face = NULL;
        }
        else
        {
            if( FT_New_Face( p_sys->p_library, psz_fontfile, i_idx, &p_face ) )
                p_face = NULL;
        }
        free( psz_fontfile );
    }
    if( !p_face )
        return NULL;
    if( FT_Select_Charmap( p_face, ft_encoding_unicode ) )
    {
        /* We've loaded a font face which is unhelpful for actually
         * rendering text - fallback to the default one.
         */
        FT_Done_Face( p_face );
        return NULL;
    }
    return p_face;
}
