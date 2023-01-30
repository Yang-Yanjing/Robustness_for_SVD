}
void vlc_epg_Clean( vlc_epg_t *p_epg )
{
    int i;
    for( i = 0; i < p_epg->i_event; i++ )
    {
        vlc_epg_event_t *p_evt = p_epg->pp_event[i];
        free( p_evt->psz_name );
        free( p_evt->psz_short_description );
        free( p_evt->psz_description );
        free( p_evt );
    }
    TAB_CLEAN( p_epg->i_event, p_epg->pp_event );
    free( p_epg->psz_name );
}
