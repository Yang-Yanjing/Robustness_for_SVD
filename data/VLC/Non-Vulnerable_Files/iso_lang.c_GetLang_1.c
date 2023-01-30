    { "Unknown", "??", "???", "???" };
const iso639_lang_t * GetLang_1( const char * psz_code )
{
    const iso639_lang_t *p_lang;
    for( p_lang = p_languages; p_lang->psz_eng_name; p_lang++ )
        if( !strncasecmp( p_lang->psz_iso639_1, psz_code, 2 ) )
            return p_lang;
    return &unknown_language;
}
