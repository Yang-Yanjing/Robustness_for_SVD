}
static filter_t *CreateFilter( vlc_object_t *p_this, es_format_t *p_fmt_in,
                               video_format_t *p_fmt_out,
                               const char *psz_module )
{
    filter_t *p_filter;
    p_filter = vlc_custom_create( p_this, sizeof(filter_t), "filter" );
    p_filter->pf_video_buffer_new =
        (picture_t *(*)(filter_t *))video_new_buffer;
    p_filter->pf_video_buffer_del =
        (void (*)(filter_t *, picture_t *))video_del_buffer;
    p_filter->fmt_in = *p_fmt_in;
    p_filter->fmt_out = *p_fmt_in;
    p_filter->fmt_out.i_codec = p_fmt_out->i_chroma;
    p_filter->fmt_out.video = *p_fmt_out;
    p_filter->p_module = module_need( p_filter, "video filter2",
                                      psz_module, false );
    if( !p_filter->p_module )
    {
        msg_Dbg( p_filter, "no video filter found" );
        DeleteFilter( p_filter );
        return NULL;
    }
    return p_filter;
}
