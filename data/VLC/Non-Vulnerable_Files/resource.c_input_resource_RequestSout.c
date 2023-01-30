/* */
sout_instance_t *input_resource_RequestSout( input_resource_t *p_resource, sout_instance_t *p_sout, const char *psz_sout )
{
    vlc_mutex_lock( &p_resource->lock );
    sout_instance_t *p_ret = RequestSout( p_resource, p_sout, psz_sout );
    vlc_mutex_unlock( &p_resource->lock );
    return p_ret;
}
