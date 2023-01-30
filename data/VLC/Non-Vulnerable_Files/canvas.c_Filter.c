 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    return filter_chain_VideoFilter( p_filter->p_sys->p_chain, p_pic );
}
