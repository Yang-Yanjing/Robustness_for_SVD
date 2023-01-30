/* */
static void DestroySout( input_resource_t *p_resource )
{
#ifdef ENABLE_SOUT
    if( p_resource->p_sout )
        sout_DeleteInstance( p_resource->p_sout );
#endif
    p_resource->p_sout = NULL;
}
