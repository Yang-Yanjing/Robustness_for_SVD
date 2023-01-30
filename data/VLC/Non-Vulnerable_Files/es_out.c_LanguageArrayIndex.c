}
static int LanguageArrayIndex( char **ppsz_langs, const char *psz_lang )
{
    if( !ppsz_langs || !psz_lang )
        return -1;
    for( int i = 0; ppsz_langs[i]; i++ )
    {
        if( !strcasecmp( ppsz_langs[i], psz_lang ) ||
            !strcasecmp( ppsz_langs[i], "any" ) )
            return i;
        if( !strcasecmp( ppsz_langs[i], "none" ) )
            break;
    }
    return -1;
}
