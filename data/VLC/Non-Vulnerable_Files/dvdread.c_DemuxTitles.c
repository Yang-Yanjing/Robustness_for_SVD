 *****************************************************************************/
static void DemuxTitles( demux_t *p_demux, int *pi_angle )
{
    VLC_UNUSED( pi_angle );
    demux_sys_t *p_sys = p_demux->p_sys;
    input_title_t *t;
    seekpoint_t *s;
    /* Find out number of titles/chapters */
#define tt_srpt p_sys->p_vmg_file->tt_srpt
    int32_t i_titles = tt_srpt->nr_of_srpts;
    msg_Dbg( p_demux, "number of titles: %d", i_titles );
    for( int i = 0; i < i_titles; i++ )
    {
        int32_t i_chapters = 0;
        int j;
        i_chapters = tt_srpt->title[i].nr_of_ptts;
        msg_Dbg( p_demux, "title %d has %d chapters", i, i_chapters );
        t = vlc_input_title_New();
        for( j = 0; j < __MAX( i_chapters, 1 ); j++ )
        {
            s = vlc_seekpoint_New();
            TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
        }
        TAB_APPEND( p_sys->i_titles, p_sys->titles, t );
    }
#undef tt_srpt
}
