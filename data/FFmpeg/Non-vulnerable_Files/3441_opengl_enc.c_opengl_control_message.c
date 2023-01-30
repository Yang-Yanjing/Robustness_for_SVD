static int opengl_control_message(AVFormatContext *h, int type, void *data, size_t data_size)
{
    OpenGLContext *opengl = h->priv_data;
    switch(type) {
    case AV_APP_TO_DEV_WINDOW_SIZE:
        if (data) {
            AVDeviceRect *message = data;
            return opengl_resize(h, message->width, message->height);
        }
        return AVERROR(EINVAL);
    case AV_APP_TO_DEV_WINDOW_REPAINT:
        return opengl_resize(h, opengl->window_width, opengl->window_height);
    }
    return AVERROR(ENOSYS);
}
