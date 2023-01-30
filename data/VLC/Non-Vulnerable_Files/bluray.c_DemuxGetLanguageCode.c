 */
static const char *DemuxGetLanguageCode( demux_t *p_demux, const char *psz_var )
{
    const iso639_lang_t *pl;
    char *psz_lang;
    char *p;
    psz_lang = var_CreateGetString( p_demux, psz_var );
    if( !psz_lang )
        return LANGUAGE_DEFAULT;
    /* XXX: we will use only the first value
     * (and ignore other ones in case of a list) */
    if( ( p = strchr( psz_lang, ',' ) ) )
        *p = '\0';
    for( pl = p_languages; pl->psz_eng_name != NULL; pl++ )
    {
        if( *psz_lang == '\0' )
            continue;
        if( !strcasecmp( pl->psz_eng_name, psz_lang ) ||
            !strcasecmp( pl->psz_iso639_1, psz_lang ) ||
            !strcasecmp( pl->psz_iso639_2T, psz_lang ) ||
            !strcasecmp( pl->psz_iso639_2B, psz_lang ) )
            break;
    }
    free( psz_lang );
    if( pl->psz_eng_name != NULL )
        return pl->psz_iso639_2T;
    return LANGUAGE_DEFAULT;
}
