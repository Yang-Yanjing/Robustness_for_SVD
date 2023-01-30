}
static vout_thread_t *HoldVout( input_resource_t *p_resource )
{
    /* TODO FIXME: p_resource->pp_vout order is NOT stable */
    vlc_mutex_lock( &p_resource->lock_hold );
    vout_thread_t *p_vout = p_resource->i_vout > 0 ? p_resource->pp_vout[0] : NULL;
    if( p_vout )
        vlc_object_hold( p_vout );
    vlc_mutex_unlock( &p_resource->lock_hold );
    return p_vout;
}
