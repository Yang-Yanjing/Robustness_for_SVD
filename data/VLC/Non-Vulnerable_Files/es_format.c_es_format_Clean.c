}
void es_format_Clean( es_format_t *fmt )
{
    free( fmt->psz_language );
    free( fmt->psz_description );
    if( fmt->i_extra > 0 ) free( fmt->p_extra );
    free( fmt->video.p_palette );
    free( fmt->subs.psz_encoding );
    if ( fmt->subs.p_style ) text_style_Delete( fmt->subs.p_style );
    if( fmt->i_extra_languages > 0 && fmt->p_extra_languages )
    {
        int i;
        for( i = 0; i < fmt->i_extra_languages; i++ )
        {
            free( fmt->p_extra_languages[i].psz_language );
            free( fmt->p_extra_languages[i].psz_description );
        }
        free( fmt->p_extra_languages );
    }
    /* es_format_Clean can be called multiple times */
    memset( fmt, 0, sizeof(*fmt) );
}
