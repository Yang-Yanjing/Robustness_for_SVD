FF_DISABLE_DEPRECATION_WARNINGS
int avfilter_ref_get_channels(AVFilterBufferRef *ref)
{
    return ref->audio ? ref->audio->channels : 0;
}
