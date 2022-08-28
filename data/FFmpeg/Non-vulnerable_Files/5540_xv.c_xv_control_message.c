static int xv_control_message(AVFormatContext *s, int type, void *data, size_t data_size)
{
    switch(type) {
    case AV_APP_TO_DEV_WINDOW_REPAINT:
        return xv_repaint(s);
    default:
        break;
    }
    return AVERROR(ENOSYS);
}
