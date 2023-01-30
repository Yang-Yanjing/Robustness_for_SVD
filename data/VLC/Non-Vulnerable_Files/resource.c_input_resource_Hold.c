}
input_resource_t *input_resource_Hold( input_resource_t *p_resource )
{
    atomic_fetch_add( &p_resource->refs, 1 );
    return p_resource;
}
