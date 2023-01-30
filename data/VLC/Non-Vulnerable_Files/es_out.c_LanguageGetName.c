 ****************************************************************************/
static char *LanguageGetName( const char *psz_code )
{
    const iso639_lang_t *pl;
    if( psz_code == NULL || !strcmp( psz_code, "und" ) )
    {
        return strdup( "" );
    }
    if( strlen( psz_code ) == 2 )
    {
        pl = GetLang_1( psz_code );
    }
    else if( strlen( psz_code ) == 3 )
    {
        pl = GetLang_2B( psz_code );
        if( !strcmp( pl->psz_iso639_1, "??" ) )
        {
            pl = GetLang_2T( psz_code );
        }
    }
    else
    {
        return strdup( psz_code );
    }
    if( !strcmp( pl->psz_iso639_1, "??" ) )
    {
       return strdup( psz_code );
    }
    else
    {
        return strdup( vlc_gettext(pl->psz_eng_name) );
    }
}
