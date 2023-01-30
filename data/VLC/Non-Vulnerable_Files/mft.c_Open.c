}
int Open(vlc_object_t *p_this)
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys;
    if (p_dec->fmt_in.i_cat != VIDEO_ES && p_dec->fmt_in.i_cat != AUDIO_ES)
        return VLC_EGENERIC;
    p_sys = p_dec->p_sys = calloc(1, sizeof(*p_sys));
    if (!p_sys)
        return VLC_ENOMEM;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (LoadMFTLibrary(&p_sys->mf_handle))
    {
        msg_Err(p_dec, "Failed to load MFT library.");
        goto error;
    }
    if (FindMFT(p_dec))
    {
        msg_Err(p_dec, "Could not find suitable MFT decoder");
        goto error;
    }
    /* Only one output sample is needed, we can allocate one and reuse it. */
    if (AllocateOutputSample(p_dec, 0, &p_sys->output_sample))
        goto error;
    if (p_sys->is_async)
    {
        p_dec->pf_decode_video = (picture_t *(*)(decoder_t *, block_t **))DecodeAsync;
        p_dec->pf_decode_audio = (block_t   *(*)(decoder_t *, block_t **))DecodeAsync;
    }
    else
    {
        p_dec->pf_decode_video = (picture_t *(*)(decoder_t *, block_t **))DecodeSync;
        p_dec->pf_decode_audio = (block_t   *(*)(decoder_t *, block_t **))DecodeSync;
    }
    p_dec->fmt_out.i_cat = p_dec->fmt_in.i_cat;
    p_dec->b_need_packetized = true;
    return VLC_SUCCESS;
error:
    Close(p_this);
    return VLC_EGENERIC;
}
