}
static void picture_unlock(picture_t *picture)
{
    picture_sys_t *pic_sys = picture->p_sys;
    vout_display_t *vd = pic_sys->vd;
    vout_display_sys_t *sys = vd->sys;
    MMAL_BUFFER_HEADER_T *buffer = pic_sys->buffer;
    vlc_mutex_lock(&sys->buffer_mutex);
    pic_sys->buffer = NULL;
    if (buffer) {
        buffer->user_data = NULL;
        mmal_buffer_header_release(buffer);
    }
    vlc_mutex_unlock(&sys->buffer_mutex);
}
