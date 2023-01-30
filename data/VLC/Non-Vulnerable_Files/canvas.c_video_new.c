 *****************************************************************************/
static picture_t *video_new( filter_t *p_filter )
{
    return filter_NewPicture( (filter_t*)p_filter->p_owner );
}
