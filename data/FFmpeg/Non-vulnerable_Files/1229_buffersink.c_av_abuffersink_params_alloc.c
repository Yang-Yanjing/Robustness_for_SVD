AVABufferSinkParams *av_abuffersink_params_alloc(void)
{
    AVABufferSinkParams *params = av_mallocz(sizeof(AVABufferSinkParams));
    if (!params)
        return NULL;
    return params;
}
