}
static void DemuxTitles( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    input_title_t *t;
    seekpoint_t *s;
    int32_t i_titles;
    int i;
    /* Menu */
    t = vlc_input_title_New();
    t->b_menu = true;
    t->psz_name = strdup( "DVD Menu" );
    s = vlc_seekpoint_New();
    s->psz_name = strdup( "Resume" );
    TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
    s = vlc_seekpoint_New();
    s->psz_name = strdup( "Root" );
    TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
    s = vlc_seekpoint_New();
    s->psz_name = strdup( "Title" );
    TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
    s = vlc_seekpoint_New();
    s->psz_name = strdup( "Chapter" );
    TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
    s = vlc_seekpoint_New();
    s->psz_name = strdup( "Subtitle" );
    TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
    s = vlc_seekpoint_New();
    s->psz_name = strdup( "Audio" );
    TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
    s = vlc_seekpoint_New();
    s->psz_name = strdup( "Angle" );
    TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
    TAB_APPEND( p_sys->i_title, p_sys->title, t );
    /* Find out number of titles/chapters */
    dvdnav_get_number_of_titles( p_sys->dvdnav, &i_titles );
    if( i_titles > 90 )
        msg_Err( p_demux, "This is probably an Arccos Protected DVD. This could take time..." );
    for( i = 1; i <= i_titles; i++ )
    {
        int32_t i_chapters;
        uint64_t i_title_length;
        uint64_t *p_chapters_time;
        i_chapters = dvdnav_describe_title_chapters( p_sys->dvdnav, i,
                                                     &p_chapters_time,
                                                     &i_title_length );
        if( i_chapters < 1 )
        {
            i_title_length = 0;
            p_chapters_time = NULL;
        }
        t = vlc_input_title_New();
        t->i_length = i_title_length * 1000 / 90;
        for( int j = 0; j < __MAX( i_chapters, 1 ); j++ )
        {
            s = vlc_seekpoint_New();
            if( p_chapters_time )
                s->i_time_offset = p_chapters_time[j] * 1000 / 90;
            TAB_APPEND( t->i_seekpoint, t->seekpoint, s );
        }
        free( p_chapters_time );
        TAB_APPEND( p_sys->i_title, p_sys->title, t );
    }
}
