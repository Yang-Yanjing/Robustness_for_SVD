}
static picture_pool_t *vd_pool(vout_display_t *vd, unsigned count)
{
    vout_display_sys_t *sys = vd->sys;
    picture_resource_t picture_res;
    picture_pool_configuration_t picture_pool_cfg;
    video_format_t fmt = vd->fmt;
    MMAL_STATUS_T status;
    unsigned i;
    if (sys->picture_pool) {
        if (sys->num_buffers < count)
            msg_Warn(vd, "Picture pool with %u pictures requested, but we already have one with %u pictures",
                            count, sys->num_buffers);
        goto out;
    }
    if (sys->opaque) {
        if (count <= NUM_ACTUAL_OPAQUE_BUFFERS)
            count = NUM_ACTUAL_OPAQUE_BUFFERS;
        else
            msg_Err(vd, "More picture (%u) than NUM_ACTUAL_OPAQUE_BUFFERS (%d) requested. Expect errors",
                            count, NUM_ACTUAL_OPAQUE_BUFFERS);
    }
    if (count < sys->input->buffer_num_recommended)
        count = sys->input->buffer_num_recommended;
#ifndef NDEBUG
    msg_Dbg(vd, "Creating picture pool with %u pictures", count);
#endif
    sys->input->buffer_num = count;
    status = mmal_port_enable(sys->input, input_port_cb);
    if (status != MMAL_SUCCESS) {
        msg_Err(vd, "Failed to enable input port %s (status=%"PRIx32" %s)",
                        sys->input->name, status, mmal_status_to_string(status));
        goto out;
    }
    status = mmal_component_enable(sys->component);
    if (status != MMAL_SUCCESS) {
        msg_Err(vd, "Failed to enable component %s (status=%"PRIx32" %s)",
                        sys->component->name, status, mmal_status_to_string(status));
        goto out;
    }
    sys->pool = mmal_pool_create_with_allocator(count, sys->input->buffer_size,
                    sys->input,
                    (mmal_pool_allocator_alloc_t)mmal_port_payload_alloc,
                    (mmal_pool_allocator_free_t)mmal_port_payload_free);
    if (!sys->pool) {
        msg_Err(vd, "Failed to create MMAL pool for %u buffers of size %"PRIu32,
                        count, sys->input->buffer_size);
        goto out;
    }
    sys->num_buffers = count;
    memset(&picture_res, 0, sizeof(picture_resource_t));
    sys->pictures = calloc(sys->num_buffers, sizeof(picture_t *));
    for (i = 0; i < sys->num_buffers; ++i) {
        picture_res.p_sys = malloc(sizeof(picture_sys_t));
        picture_res.p_sys->vd = vd;
        picture_res.p_sys->buffer = NULL;
        sys->pictures[i] = picture_NewFromResource(&fmt, &picture_res);
        if (!sys->pictures[i]) {
            msg_Err(vd, "Failed to create picture");
            free(picture_res.p_sys);
            goto out;
        }
    }
    memset(&picture_pool_cfg, 0, sizeof(picture_pool_configuration_t));
    picture_pool_cfg.picture_count = sys->num_buffers;
    picture_pool_cfg.picture = sys->pictures;
    picture_pool_cfg.lock = picture_lock;
    picture_pool_cfg.unlock = picture_unlock;
    sys->picture_pool = picture_pool_NewExtended(&picture_pool_cfg);
    if (!sys->picture_pool) {
        msg_Err(vd, "Failed to create picture pool");
        goto out;
    }
out:
    return sys->picture_pool;
}
