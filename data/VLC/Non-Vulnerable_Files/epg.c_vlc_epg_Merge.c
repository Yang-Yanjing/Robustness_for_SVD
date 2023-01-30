}
void vlc_epg_Merge( vlc_epg_t *p_dst, const vlc_epg_t *p_src )
{
    int i;
    /* Add new event */
    for( i = 0; i < p_src->i_event; i++ )
    {
        vlc_epg_event_t *p_evt = p_src->pp_event[i];
        bool b_add = true;
        int j;
        for( j = 0; j < p_dst->i_event; j++ )
        {
            if( p_dst->pp_event[j]->i_start == p_evt->i_start && p_dst->pp_event[j]->i_duration == p_evt->i_duration )
            {
                b_add = false;
                break;
            }
            if( p_dst->pp_event[j]->i_start > p_evt->i_start )
                break;
        }
        if( b_add )
        {
            vlc_epg_event_t *p_copy = calloc( 1, sizeof(vlc_epg_event_t) );
            if( !p_copy )
                break;
            p_copy->i_start = p_evt->i_start;
            p_copy->i_duration = p_evt->i_duration;
            p_copy->psz_name = p_evt->psz_name ? strdup( p_evt->psz_name ) : NULL;
            p_copy->psz_short_description = p_evt->psz_short_description ? strdup( p_evt->psz_short_description ) : NULL;
            p_copy->psz_description = p_evt->psz_description ? strdup( p_evt->psz_description ) : NULL;
            p_copy->i_rating = p_evt->i_rating;
            TAB_INSERT( p_dst->i_event, p_dst->pp_event, p_copy, j );
        }
    }
    /* Update current */
    if( p_src->p_current )
        vlc_epg_SetCurrent( p_dst, p_src->p_current->i_start );
    /* Keep only 1 old event  */
    if( p_dst->p_current )
    {
        while( p_dst->i_event > 1 && p_dst->pp_event[0] != p_dst->p_current && p_dst->pp_event[1] != p_dst->p_current )
            TAB_REMOVE( p_dst->i_event, p_dst->pp_event, p_dst->pp_event[0] );
    }
}
