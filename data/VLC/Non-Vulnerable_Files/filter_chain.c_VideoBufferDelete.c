}
static void VideoBufferDelete( filter_t *p_filter, picture_t *p_picture )
{
    VLC_UNUSED( p_filter );
    picture_Release( p_picture );
}
