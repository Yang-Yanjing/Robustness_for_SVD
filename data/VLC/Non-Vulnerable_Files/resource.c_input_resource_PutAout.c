}
void input_resource_PutAout( input_resource_t *p_resource,
                             audio_output_t *p_aout )
{
    assert( p_aout != NULL );
    vlc_mutex_lock( &p_resource->lock_hold );
    if( p_aout == p_resource->p_aout )
    {
        assert( p_resource->b_aout_busy );
        p_resource->b_aout_busy = false;
        msg_Dbg( p_resource->p_parent, "keeping audio output" );
        p_aout = NULL;
    }
    else
        msg_Dbg( p_resource->p_parent, "destroying extra audio output" );
    vlc_mutex_unlock( &p_resource->lock_hold );
    if( p_aout != NULL )
        aout_Destroy( p_aout );
}
