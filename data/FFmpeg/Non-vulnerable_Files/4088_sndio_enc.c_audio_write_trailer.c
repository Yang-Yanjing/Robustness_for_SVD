static int audio_write_trailer(AVFormatContext *s1)
{
    SndioData *s = s1->priv_data;
    sio_write(s->hdl, s->buffer, s->buffer_offset);
    ff_sndio_close(s);
    return 0;
}
