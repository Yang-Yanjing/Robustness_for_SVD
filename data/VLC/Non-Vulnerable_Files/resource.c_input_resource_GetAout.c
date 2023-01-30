/* Audio output */
audio_output_t *input_resource_GetAout( input_resource_t *p_resource )
{
    audio_output_t *p_aout;
    vlc_mutex_lock( &p_resource->lock_hold );
    p_aout = p_resource->p_aout;
    if( p_aout == NULL || p_resource->b_aout_busy )
    {
        msg_Dbg( p_resource->p_parent, "creating audio output" );
        vlc_mutex_unlock( &p_resource->lock_hold );
        p_aout = aout_New( p_resource->p_parent );
        if( p_aout == NULL )
            return NULL; /* failed */
        vlc_mutex_lock( &p_resource->lock_hold );
        if( p_resource->p_aout == NULL )
            p_resource->p_aout = p_aout;
    }
    else
        msg_Dbg( p_resource->p_parent, "reusing audio output" );
    if( p_resource->p_aout == p_aout )
    {
        assert( !p_resource->b_aout_busy );
        p_resource->b_aout_busy = true;
    }
    vlc_mutex_unlock( &p_resource->lock_hold );
    return p_aout;
}
