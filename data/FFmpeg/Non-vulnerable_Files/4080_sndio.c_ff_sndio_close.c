int ff_sndio_close(SndioData *s)
{
    av_freep(&s->buffer);
    if (s->hdl)
        sio_close(s->hdl);
    return 0;
}
