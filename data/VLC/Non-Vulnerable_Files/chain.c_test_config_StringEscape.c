};
static void test_config_StringEscape()
{
    for( int i = 0; escape_samples[i].psz_string; i++ )
    {
        char *psz_tmp = config_StringEscape( escape_samples[i].psz_string );
        assert( !strcmp( psz_tmp, escape_samples[i].psz_escaped ) );
        free( psz_tmp );
    }
}
