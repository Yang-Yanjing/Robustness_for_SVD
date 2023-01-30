}
static void video_del( filter_t *p_filter, picture_t *p_pic )
{
    return filter_DeletePicture( (filter_t*)p_filter->p_owner, p_pic );
}
