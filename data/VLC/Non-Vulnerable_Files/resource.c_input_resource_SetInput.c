}
void input_resource_SetInput( input_resource_t *p_resource, input_thread_t *p_input )
{
    vlc_mutex_lock( &p_resource->lock );
    if( p_resource->p_input && !p_input )
        assert( p_resource->i_vout == 0 );
    /* */
    p_resource->p_input = p_input;
    vlc_mutex_unlock( &p_resource->lock );
}
