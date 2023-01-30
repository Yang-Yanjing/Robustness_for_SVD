/* Common */
input_resource_t *input_resource_New( vlc_object_t *p_parent )
{
    input_resource_t *p_resource = calloc( 1, sizeof(*p_resource) );
    if( !p_resource )
        return NULL;
    atomic_init( &p_resource->refs, 1 );
    p_resource->p_parent = p_parent;
    vlc_mutex_init( &p_resource->lock );
    vlc_mutex_init( &p_resource->lock_hold );
    return p_resource;
}
