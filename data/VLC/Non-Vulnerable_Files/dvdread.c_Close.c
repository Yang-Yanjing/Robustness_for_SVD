 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    int i;
    for( i = 0; i < PS_TK_COUNT; i++ )
    {
        ps_track_t *tk = &p_sys->tk[i];
        if( tk->b_seen )
        {
            es_format_Clean( &tk->fmt );
            if( tk->es ) es_out_Del( p_demux->out, tk->es );
        }
    }
    /* Free the array of titles */
    for( int i = 0; i < p_sys->i_titles; i++ )
        vlc_input_title_Delete( p_sys->titles[i] );
    TAB_CLEAN( p_sys->i_titles, p_sys->titles );
    /* Close libdvdread */
    if( p_sys->p_title ) DVDCloseFile( p_sys->p_title );
    if( p_sys->p_vts_file ) ifoClose( p_sys->p_vts_file );
    if( p_sys->p_vmg_file ) ifoClose( p_sys->p_vmg_file );
    DVDClose( p_sys->p_dvdread );
    free( p_sys );
}
