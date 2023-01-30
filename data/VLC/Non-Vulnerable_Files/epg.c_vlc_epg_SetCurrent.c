}
void vlc_epg_SetCurrent( vlc_epg_t *p_epg, int64_t i_start )
{
    int i;
    p_epg->p_current = NULL;
    if( i_start < 0 )
        return;
    for( i = 0; i < p_epg->i_event; i++ )
    {
        if( p_epg->pp_event[i]->i_start == i_start )
        {
            p_epg->p_current = p_epg->pp_event[i];
            break;
        }
    }
}
