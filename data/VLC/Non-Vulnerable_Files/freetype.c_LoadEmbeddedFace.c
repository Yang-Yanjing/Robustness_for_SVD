}
static FT_Face LoadEmbeddedFace( filter_sys_t *p_sys, const text_style_t *p_style )
{
    for( int k = 0; k < p_sys->i_font_attachments; k++ )
    {
        input_attachment_t *p_attach   = p_sys->pp_font_attachments[k];
        int                 i_font_idx = 0;
        FT_Face             p_face = NULL;
        while( 0 == FT_New_Memory_Face( p_sys->p_library,
                                        p_attach->p_data,
                                        p_attach->i_data,
                                        i_font_idx,
                                        &p_face ))
        {
            if( p_face )
            {
                int i_style_received = ((p_face->style_flags & FT_STYLE_FLAG_BOLD)    ? STYLE_BOLD   : 0) |
                                       ((p_face->style_flags & FT_STYLE_FLAG_ITALIC ) ? STYLE_ITALIC : 0);
                if( p_face->family_name != NULL
                 && !strcasecmp( p_face->family_name, p_style->psz_fontname )
                 && (p_style->i_style_flags & (STYLE_BOLD | STYLE_ITALIC))
                                                          == i_style_received )
                    return p_face;
                FT_Done_Face( p_face );
            }
            i_font_idx++;
        }
    }
    return NULL;
}
