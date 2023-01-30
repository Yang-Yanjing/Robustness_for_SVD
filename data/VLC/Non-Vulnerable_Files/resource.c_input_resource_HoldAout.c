}
audio_output_t *input_resource_HoldAout( input_resource_t *p_resource )
{
    audio_output_t *p_aout;
    vlc_mutex_lock( &p_resource->lock_hold );
    p_aout = p_resource->p_aout;
    if( p_aout != NULL )
        vlc_object_hold( p_aout );
    vlc_mutex_unlock( &p_resource->lock_hold );
    return p_aout;
}
