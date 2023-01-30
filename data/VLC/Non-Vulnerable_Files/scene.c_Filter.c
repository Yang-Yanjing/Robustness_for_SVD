 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    /* TODO: think of some funky algorithm to detect scene changes. */
    SnapshotRatio( p_filter, p_pic );
    return p_pic;
}
