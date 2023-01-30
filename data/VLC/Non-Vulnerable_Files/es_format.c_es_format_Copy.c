}
int es_format_Copy( es_format_t *dst, const es_format_t *src )
{
    int i;
    memcpy( dst, src, sizeof( es_format_t ) );
    dst->psz_language = src->psz_language ? strdup( src->psz_language ) : NULL;
    dst->psz_description = src->psz_description ? strdup( src->psz_description ) : NULL;
    if( src->i_extra > 0 && dst->p_extra )
    {
        dst->i_extra = src->i_extra;
        dst->p_extra = malloc( src->i_extra );
        if(dst->p_extra)
            memcpy( dst->p_extra, src->p_extra, src->i_extra );
        else
            dst->i_extra = 0;
    }
    else
    {
        dst->i_extra = 0;
        dst->p_extra = NULL;
    }
    dst->subs.psz_encoding = dst->subs.psz_encoding ? strdup( src->subs.psz_encoding ) : NULL;
    dst->subs.p_style = src->subs.p_style ? text_style_Duplicate( src->subs.p_style ) : NULL;
    if( src->video.p_palette )
    {
        dst->video.p_palette =
            (video_palette_t*)malloc( sizeof( video_palette_t ) );
        if(dst->video.p_palette)
        {
            memcpy( dst->video.p_palette, src->video.p_palette,
                sizeof( video_palette_t ) );
        }
    }
    if( dst->i_extra_languages && src->p_extra_languages)
    {
        dst->i_extra_languages = src->i_extra_languages;
        dst->p_extra_languages = (extra_languages_t*)
            malloc(dst->i_extra_languages * sizeof(*dst->p_extra_languages ));
        if( dst->p_extra_languages )
        {
            for( i = 0; i < dst->i_extra_languages; i++ ) {
                if( src->p_extra_languages[i].psz_language )
                    dst->p_extra_languages[i].psz_language = strdup( src->p_extra_languages[i].psz_language );
                else
                    dst->p_extra_languages[i].psz_language = NULL;
                if( src->p_extra_languages[i].psz_description )
                    dst->p_extra_languages[i].psz_description = strdup( src->p_extra_languages[i].psz_description );
                else
                    dst->p_extra_languages[i].psz_description = NULL;
            }
        }
        else
            dst->i_extra_languages = 0;
    }
    else
        dst->i_extra_languages = 0;
    return VLC_SUCCESS;
}
