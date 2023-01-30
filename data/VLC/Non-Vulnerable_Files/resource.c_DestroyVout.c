/* */
static void DestroyVout( input_resource_t *p_resource )
{
    assert( p_resource->i_vout == 0 );
    if( p_resource->p_vout_free )
        vout_CloseAndRelease( p_resource->p_vout_free );
    p_resource->p_vout_free = NULL;
}
