static block_t *Process (filter_t *, block_t *);
static int Open (vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    if (filter->fmt_in.audio.i_channels != 2)
    {
        msg_Err (filter, "voice removal requires stereo");
        return VLC_EGENERIC;
    }
    filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    filter->fmt_out.audio = filter->fmt_in.audio;
    filter->pf_audio_filter = Process;
    return VLC_SUCCESS;
}
