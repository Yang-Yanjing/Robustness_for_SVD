#endif
static void InitTitle( input_thread_t * p_input )
{
    input_source_t *p_master = &p_input->p->input;
    if( p_input->b_preparsing )
        return;
    vlc_mutex_lock( &p_input->p->p_item->lock );
    /* Create global title (from master) */
    p_input->p->i_title = p_master->i_title;
    p_input->p->title   = p_master->title;
    p_input->p->i_title_offset = p_master->i_title_offset;
    p_input->p->i_seekpoint_offset = p_master->i_seekpoint_offset;
    if( p_input->p->i_title > 0 )
    {
        /* Setup variables */
        input_ControlVarNavigation( p_input );
        input_SendEventTitle( p_input, 0 );
    }
    /* Global flag */
    p_input->p->b_can_pace_control    = p_master->b_can_pace_control;
    p_input->p->b_can_pause        = p_master->b_can_pause;
    p_input->p->b_can_rate_control = p_master->b_can_rate_control;
    vlc_mutex_unlock( &p_input->p->p_item->lock );
}
