}
static int picture_lock(picture_t *picture)
{
    vout_display_t *vd = picture->p_sys->vd;
    vout_display_sys_t *sys = vd->sys;
    picture_sys_t *pic_sys = picture->p_sys;
    MMAL_BUFFER_HEADER_T *buffer = mmal_queue_wait(sys->pool->queue);
    if (!buffer)
        return VLC_EGENERIC;
    vlc_mutex_lock(&sys->buffer_mutex);
    mmal_buffer_header_reset(buffer);
    buffer->user_data = picture;
    pic_sys->buffer = buffer;
    memcpy(picture->p, sys->planes, sizeof(sys->planes));
    picture->p[0].p_pixels = buffer->data;
    picture->p[1].p_pixels += (ptrdiff_t)buffer->data;
    picture->p[2].p_pixels += (ptrdiff_t)buffer->data;
    pic_sys->displayed = false;
    vlc_mutex_unlock(&sys->buffer_mutex);
    return VLC_SUCCESS;
}
