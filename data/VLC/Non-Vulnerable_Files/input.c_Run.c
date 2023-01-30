 *****************************************************************************/
static void *Run( void *obj )
{
    input_thread_t *p_input = (input_thread_t *)obj;
    const int canc = vlc_savecancel();
    if( Init( p_input ) )
        goto exit;
    MainLoop( p_input, true ); /* FIXME it can be wrong (like with VLM) */
    /* Clean up */
    End( p_input );
exit:
    /* Tell we're dead */
    vlc_mutex_lock( &p_input->p->lock_control );
    const bool b_abort = p_input->p->b_abort;
    vlc_mutex_unlock( &p_input->p->lock_control );
    if( b_abort )
        input_SendEventAbort( p_input );
    input_SendEventDead( p_input );
    vlc_restorecancel( canc );
    return NULL;
}
