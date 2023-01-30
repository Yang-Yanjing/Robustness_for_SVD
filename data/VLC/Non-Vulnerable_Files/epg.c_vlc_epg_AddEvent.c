}
void vlc_epg_AddEvent( vlc_epg_t *p_epg, int64_t i_start, int i_duration,
                       const char *psz_name, const char *psz_short_description,
                       const char *psz_description, uint8_t i_rating )
{
    vlc_epg_event_t *p_evt = malloc( sizeof(*p_evt) );
    if( !p_evt )
        return;
    p_evt->i_start = i_start;
    p_evt->i_duration = i_duration;
    p_evt->psz_name = psz_name ? strdup( psz_name ) : NULL;
    p_evt->psz_short_description = psz_short_description ? strdup( psz_short_description ) : NULL;
    p_evt->psz_description = psz_description ? strdup( psz_description ) : NULL;
    p_evt->i_rating = i_rating;
    TAB_APPEND( p_epg->i_event, p_epg->pp_event, p_evt );
}
