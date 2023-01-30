 */
static void Destructor( input_thread_t * p_input )
{
#ifndef NDEBUG
    char * psz_name = input_item_GetName( p_input->p->p_item );
    msg_Dbg( p_input, "Destroying the input for '%s'", psz_name);
    free( psz_name );
#endif
    if( p_input->p->p_es_out_display )
        es_out_Delete( p_input->p->p_es_out_display );
    if( p_input->p->p_resource )
        input_resource_Release( p_input->p->p_resource );
    if( p_input->p->p_resource_private )
        input_resource_Release( p_input->p->p_resource_private );
    vlc_gc_decref( p_input->p->p_item );
    vlc_mutex_destroy( &p_input->p->counters.counters_lock );
    for( int i = 0; i < p_input->p->i_control; i++ )
    {
        input_control_t *p_ctrl = &p_input->p->control[i];
        ControlRelease( p_ctrl->i_type, p_ctrl->val );
    }
    vlc_cond_destroy( &p_input->p->wait_control );
    vlc_mutex_destroy( &p_input->p->lock_control );
    free( p_input->p );
}
