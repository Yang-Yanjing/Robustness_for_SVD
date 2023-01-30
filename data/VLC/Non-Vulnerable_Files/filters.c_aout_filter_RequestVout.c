}
vout_thread_t *aout_filter_RequestVout (filter_t *filter, vout_thread_t *vout,
                                        video_format_t *fmt)
{
    /* NOTE: This only works from aout_filters_t.
     * If you want to use visualization filters from another place, you will
     * need to add a new pf_aout_request_vout callback or store a pointer
     * to aout_request_vout_t inside filter_t (i.e. a level of indirection). */
    const aout_request_vout_t *req = (void *)filter->p_owner;
    char *visual = var_InheritString (filter->p_parent, "audio-visual");
    /* NOTE: Disable recycling to always close the filter vout because OpenGL
     * visualizations do not use this function to ask for a context. */
    bool recycle = false;
    free (visual);
    return req->pf_request_vout (req->p_private, vout, fmt, recycle);
}
