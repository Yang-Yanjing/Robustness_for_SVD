};
static void test_config_StringEscape()
{
    for( int i = 0; md5_samples[i].psz_string; i++ )
    {
        struct md5_s md5;
        InitMD5( &md5 );
        AddMD5( &md5, md5_samples[i].psz_string, strlen( md5_samples[i].psz_string ) );
        EndMD5( &md5 );
        char * psz_hash = psz_md5_hash( &md5 );
        if( strcmp( psz_hash, md5_samples[i].psz_md5 ) )
        {
            printf( "Output: %s\nExpected: %s\n", psz_hash,
                    md5_samples[i].psz_md5 );
            abort();
        }
        free( psz_hash );
    }
}
