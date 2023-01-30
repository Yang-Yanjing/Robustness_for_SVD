#define BD_45KHZ INT64_C(45000)
static void InitTitles( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    /* */
    for( int i = 0; i < p_sys->i_mpls; i++ )
    {
        const bd_mpls_t *p_mpls = p_sys->pp_mpls[i];
        input_title_t *t = vlc_input_title_New();
        if( !t )
            break;
        /* */
        t->i_length = 0;
        for( int j = 0; j < p_mpls->i_play_item; j++ )
        {
            const bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[j];
            t->i_length += ( p_item->i_out_time - p_item->i_in_time ) * CLOCK_FREQ / BD_45KHZ;
        }
#ifdef BD_DEBUG
        {
        char psz_length[MSTRTIME_MAX_SIZE];
        msg_Warn( p_demux, "TITLE[%d] %s", i, secstotimestr( psz_length, t->i_length / CLOCK_FREQ ) );
        }
#endif
        /* Seekpoint */
        for( int j = 0; j < p_mpls->i_mark; j++ )
        {
            bd_mpls_mark_t *p_mark = &p_mpls->p_mark[j];
            if( p_mark->i_type == BD_MPLS_MARK_TYPE_BOOKMARK &&
                p_mark->i_play_item_id >= 0 && p_mark->i_play_item_id < p_mpls->i_play_item )
            {
                seekpoint_t *s = vlc_seekpoint_New();
                if( !s )
                    break;
                for( int k = 0; k <= p_mark->i_play_item_id; k++ )
                {
                    const bd_mpls_play_item_t *p_item = &p_mpls->p_play_item[k];
                    int64_t i_out_time;
                    if( k == p_mark->i_play_item_id )
                        i_out_time = p_mark->i_time;
                    else
                        i_out_time = p_item->i_out_time;
                    s->i_time_offset += ( i_out_time - p_item->i_in_time ) * CLOCK_FREQ / BD_45KHZ;
                }
#ifdef BD_DEBUG
                {
                char psz_time[MSTRTIME_MAX_SIZE];
                msg_Warn( p_demux, "    SEEKPOINT[%d] %s", j, secstotimestr( psz_time, s->i_time_offset / CLOCK_FREQ ) );
                }
#endif
                TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
            }
        }
        if( t->i_seekpoint <= 0 )
        {
            seekpoint_t *s = vlc_seekpoint_New();
            if( s )
                TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
        }
        TAB_APPEND( p_sys->i_title, p_sys->pp_title, t );
    }
}
