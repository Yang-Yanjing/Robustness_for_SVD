}
static void HoldVouts( input_resource_t *p_resource, vout_thread_t ***ppp_vout,
                       size_t *pi_vout )
{
    vout_thread_t **pp_vout;
    *pi_vout = 0;
    *ppp_vout = NULL;
    vlc_mutex_lock( &p_resource->lock_hold );
    if( p_resource->i_vout <= 0 )
        goto exit;
    pp_vout = malloc( p_resource->i_vout * sizeof(*pp_vout) );
    if( !pp_vout )
        goto exit;
    *ppp_vout = pp_vout;
    *pi_vout = p_resource->i_vout;
    for( int i = 0; i < p_resource->i_vout; i++ )
    {
        pp_vout[i] = p_resource->pp_vout[i];
        vlc_object_hold( pp_vout[i] );
    }
exit:
    vlc_mutex_unlock( &p_resource->lock_hold );
}
