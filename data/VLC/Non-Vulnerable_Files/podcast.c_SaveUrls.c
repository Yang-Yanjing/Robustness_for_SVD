}
static void SaveUrls( services_discovery_t *p_sd )
{
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    int i;
    char *psz_urls;
    int len = 0;
    for( i=0; i < p_sys->i_urls; i++ )
        len += strlen( p_sys->ppsz_urls[i] ) + 1;
    psz_urls = (char*) calloc( len, sizeof(char) );
    for( i=0; i < p_sys->i_urls; i++ )
    {
        strcat( psz_urls, p_sys->ppsz_urls[i] );
        if( i < p_sys->i_urls - 1 ) strcat( psz_urls, "|" );
    }
    config_PutPsz( p_sd, "podcast-urls", psz_urls );
    free( psz_urls );
}
