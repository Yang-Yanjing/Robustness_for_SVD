#include <vlc_epg.h>
void vlc_epg_Init( vlc_epg_t *p_epg, const char *psz_name )
{
    p_epg->psz_name = psz_name ? strdup( psz_name ) : NULL;
    p_epg->p_current = NULL;
    TAB_INIT( p_epg->i_event, p_epg->pp_event );
}
