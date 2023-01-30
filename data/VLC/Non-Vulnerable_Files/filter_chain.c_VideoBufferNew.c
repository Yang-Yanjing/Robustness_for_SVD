/* Internal video allocator functions */
static picture_t *VideoBufferNew( filter_t *p_filter )
{
    const video_format_t *p_fmt = &p_filter->fmt_out.video;
    picture_t *p_picture = picture_NewFromFormat( p_fmt );
    if( !p_picture )
        msg_Err( p_filter, "Failed to allocate picture" );
    return p_picture;
}
