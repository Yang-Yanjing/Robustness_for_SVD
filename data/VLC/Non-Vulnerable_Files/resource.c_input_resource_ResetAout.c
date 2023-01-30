}
void input_resource_ResetAout( input_resource_t *p_resource )
{
    audio_output_t *p_aout = NULL;
    vlc_mutex_lock( &p_resource->lock_hold );
    if( !p_resource->b_aout_busy )
        p_aout = p_resource->p_aout;
    p_resource->p_aout = NULL;
    p_resource->b_aout_busy = false;
    vlc_mutex_unlock( &p_resource->lock_hold );
    if( p_aout != NULL )
        aout_Destroy( p_aout );
}
