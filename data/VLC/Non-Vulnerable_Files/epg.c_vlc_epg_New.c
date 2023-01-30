}
vlc_epg_t *vlc_epg_New( const char *psz_name )
{
    vlc_epg_t *p_epg = malloc( sizeof(*p_epg) );
    if( p_epg )
        vlc_epg_Init( p_epg, psz_name );
    return p_epg;
}
