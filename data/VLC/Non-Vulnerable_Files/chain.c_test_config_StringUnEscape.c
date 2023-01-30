}
static void test_config_StringUnEscape()
{
    for( int i = 0; escape_samples[i].psz_string; i++ )
    {
        char *psz_tmp = strdup( escape_samples[i].psz_escaped );
        config_StringUnescape( psz_tmp );
        assert( !strcmp( psz_tmp, escape_samples[i].psz_string ) );
        free( psz_tmp );
    }
}
