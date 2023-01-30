}
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;
    unsigned i;
    vc_tv_unregister_callback_full(tvservice_cb, vd);
    if (sys->dmx_handle)
        close_dmx(vd);
    if (sys->component && sys->component->control->is_enabled)
        mmal_port_disable(sys->component->control);
    if (sys->input && sys->input->is_enabled)
        mmal_port_disable(sys->input);
    if (sys->component && sys->component->is_enabled)
        mmal_component_disable(sys->component);
    if (sys->pool)
        mmal_pool_destroy(sys->pool);
    if (sys->component)
        mmal_component_release(sys->component);
    if (sys->picture_pool)
        picture_pool_Delete(sys->picture_pool);
    else
        for (i = 0; i < sys->num_buffers; ++i)
            if (sys->pictures[i])
                picture_Release(sys->pictures[i]);
    vlc_mutex_destroy(&sys->buffer_mutex);
    vlc_cond_destroy(&sys->buffer_cond);
    vlc_mutex_destroy(&sys->manage_mutex);
    free(sys);
    bcm_host_deinit();
}
