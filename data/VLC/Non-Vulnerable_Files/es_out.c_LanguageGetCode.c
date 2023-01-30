/* Get a 2 char code */
static char *LanguageGetCode( const char *psz_lang )
{
    const iso639_lang_t *pl;
    if( psz_lang == NULL || *psz_lang == '\0' )
        return strdup("??");
    for( pl = p_languages; pl->psz_eng_name != NULL; pl++ )
    {
        if( !strcasecmp( pl->psz_eng_name, psz_lang ) ||
            !strcasecmp( pl->psz_iso639_1, psz_lang ) ||
            !strcasecmp( pl->psz_iso639_2T, psz_lang ) ||
            !strcasecmp( pl->psz_iso639_2B, psz_lang ) )
            return strdup( pl->psz_iso639_1 );
    }
    return strdup("??");
}
