 *****************************************************************************/
static void I422_YUVA( filter_t *p_filter, picture_t *p_source,
                                           picture_t *p_dest )
{
    I422_I420( p_filter, p_source, p_dest );
    memset( p_dest->p[A_PLANE].p_pixels, 0xff,
                p_dest->p[A_PLANE].i_lines * p_dest->p[A_PLANE].i_pitch );
}
