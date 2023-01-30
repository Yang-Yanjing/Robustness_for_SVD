static int audio_read_close(AVFormatContext *context)
{
    JackData *self = context->priv_data;
    stop_jack(self);
    return 0;
}
