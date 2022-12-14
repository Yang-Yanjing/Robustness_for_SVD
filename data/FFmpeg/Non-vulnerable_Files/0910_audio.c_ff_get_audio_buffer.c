AVFrame *ff_get_audio_buffer(AVFilterLink *link, int nb_samples)
{
    AVFrame *ret = NULL;
    if (link->dstpad->get_audio_buffer)
        ret = link->dstpad->get_audio_buffer(link, nb_samples);
    if (!ret)
        ret = ff_default_get_audio_buffer(link, nb_samples);
    return ret;
}
