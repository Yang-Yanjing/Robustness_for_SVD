}
static void input_port_cb(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
    vout_display_t *vd = (vout_display_t *)port->userdata;
    vout_display_sys_t *sys = vd->sys;
    picture_t *picture = (picture_t *)buffer->user_data;
    vlc_mutex_lock(&sys->buffer_mutex);
    --sys->buffers_in_transit;
    vlc_cond_signal(&sys->buffer_cond);
    vlc_mutex_unlock(&sys->buffer_mutex);
    if (picture)
        picture_Release(picture);
}
