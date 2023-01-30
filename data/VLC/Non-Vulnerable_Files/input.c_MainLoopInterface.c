 */
static void MainLoopInterface( input_thread_t *p_input )
{
    double f_position = 0.0;
    mtime_t i_time = 0;
    mtime_t i_length = 0;
    /* update input status variables */
    if( demux_Control( p_input->p->input.p_demux,
                       DEMUX_GET_POSITION, &f_position ) )
        f_position = 0.0;
    if( demux_Control( p_input->p->input.p_demux,
                       DEMUX_GET_TIME, &i_time ) )
        i_time = 0;
    p_input->p->i_time = i_time;
    if( demux_Control( p_input->p->input.p_demux,
                       DEMUX_GET_LENGTH, &i_length ) )
        i_length = 0;
    es_out_SetTimes( p_input->p->p_es_out, f_position, i_time, i_length );
    /* update current bookmark */
    vlc_mutex_lock( &p_input->p->p_item->lock );
    p_input->p->bookmark.i_time_offset = i_time;
    p_input->p->bookmark.i_byte_offset = -1;
    vlc_mutex_unlock( &p_input->p->p_item->lock );
}
