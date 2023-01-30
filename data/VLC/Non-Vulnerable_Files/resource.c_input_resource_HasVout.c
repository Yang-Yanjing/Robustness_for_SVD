}
bool input_resource_HasVout( input_resource_t *p_resource )
{
    vlc_mutex_lock( &p_resource->lock );
    assert( !p_resource->p_input );
    const bool b_vout = p_resource->p_vout_free != NULL;
    vlc_mutex_unlock( &p_resource->lock );
    return b_vout;
}
