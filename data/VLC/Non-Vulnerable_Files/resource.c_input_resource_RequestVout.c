}
vout_thread_t *input_resource_RequestVout( input_resource_t *p_resource,
                                            vout_thread_t *p_vout,
                                            video_format_t *p_fmt, unsigned dpb_size,
                                            bool b_recycle )
{
    vlc_mutex_lock( &p_resource->lock );
    vout_thread_t *p_ret = RequestVout( p_resource, p_vout, p_fmt, dpb_size, b_recycle );
    vlc_mutex_unlock( &p_resource->lock );
    return p_ret;
}
