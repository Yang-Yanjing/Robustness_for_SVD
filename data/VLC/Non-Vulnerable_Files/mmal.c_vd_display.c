}
static void vd_display(vout_display_t *vd, picture_t *picture,
                subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    picture_sys_t *pic_sys = picture->p_sys;
    MMAL_BUFFER_HEADER_T *buffer = pic_sys->buffer;
    MMAL_STATUS_T status;
    if (!pic_sys->displayed || !sys->opaque) {
        buffer->cmd = 0;
        buffer->length = sys->input->buffer_size;
        vlc_mutex_lock(&sys->buffer_mutex);
        while (sys->buffers_in_transit >= MAX_BUFFERS_IN_TRANSIT)
            vlc_cond_wait(&sys->buffer_cond, &sys->buffer_mutex);
        status = mmal_port_send_buffer(sys->input, buffer);
        if (status == MMAL_SUCCESS)
            ++sys->buffers_in_transit;
        vlc_mutex_unlock(&sys->buffer_mutex);
        if (status != MMAL_SUCCESS) {
            msg_Err(vd, "Failed to send buffer to input port. Frame dropped");
            picture_Release(picture);
        }
        pic_sys->displayed = true;
    } else {
        picture_Release(picture);
    }
    display_subpicture(vd, subpicture);
    if (subpicture)
        subpicture_Delete(subpicture);
}
