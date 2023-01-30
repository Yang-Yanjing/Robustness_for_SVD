void avfilter_unref_bufferp(AVFilterBufferRef **ref)
{
FF_DISABLE_DEPRECATION_WARNINGS
    avfilter_unref_buffer(*ref);
FF_ENABLE_DEPRECATION_WARNINGS
    *ref = NULL;
}
