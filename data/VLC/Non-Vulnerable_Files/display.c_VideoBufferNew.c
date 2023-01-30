 *****************************************************************************/
static picture_t *VideoBufferNew(filter_t *filter)
{
    vout_display_t *vd = (vout_display_t*)filter->p_owner;
    const video_format_t *fmt = &filter->fmt_out.video;
    assert(vd->fmt.i_chroma == fmt->i_chroma &&
           vd->fmt.i_width  == fmt->i_width  &&
           vd->fmt.i_height == fmt->i_height);
    picture_pool_t *pool = vout_display_Pool(vd, 3);
    if (!pool)
        return NULL;
    return picture_pool_Get(pool);
}
