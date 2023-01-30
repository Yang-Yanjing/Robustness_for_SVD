}
void input_resource_Release( input_resource_t *p_resource )
{
    if( atomic_fetch_sub( &p_resource->refs, 1 ) != 1 )
        return;
    DestroySout( p_resource );
    DestroyVout( p_resource );
    if( p_resource->p_aout != NULL )
        aout_Destroy( p_resource->p_aout );
    vlc_mutex_destroy( &p_resource->lock_hold );
    vlc_mutex_destroy( &p_resource->lock );
    free( p_resource );
}
