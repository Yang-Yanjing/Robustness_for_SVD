}
static void UpdateTitleListfromDemux( input_thread_t *p_input )
{
    input_source_t *in = &p_input->p->input;
    /* Delete the preexisting titles */
    if( in->i_title > 0 )
    {
        for( int i = 0; i < in->i_title; i++ )
            vlc_input_title_Delete( in->title[i] );
        TAB_CLEAN( in->i_title, in->title );
        in->b_title_demux = false;
    }
    /* Get the new title list */
    if( demux_Control( in->p_demux, DEMUX_GET_TITLE_INFO,
                       &in->title, &in->i_title,
                       &in->i_title_offset, &in->i_seekpoint_offset ) )
        TAB_INIT( in->i_title, in->title );
    else
        in->b_title_demux = true;
    InitTitle( p_input );
}
