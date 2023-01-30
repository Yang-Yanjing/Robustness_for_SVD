 */
static void FieldFromPlane( plane_t *p_dst, const plane_t *p_src, int i_field )
{
    assert( p_dst != NULL );
    assert( p_src != NULL );
    assert( i_field == 0  ||  i_field == 1 );
    /* Start with a copy of the metadata, and then update it to refer
       to one field only.
       We utilize the fact that plane_CopyPixels() differentiates between
       visible_pitch and pitch.
       The other field will be defined as the "margin" by doubling the pitch.
       The visible pitch will be left as in the original.
    */
    (*p_dst) = (*p_src);
    p_dst->i_lines /= 2;
    p_dst->i_visible_lines /= 2;
    p_dst->i_pitch *= 2;
    /* For the bottom field, skip the first line in the pixel data. */
    if( i_field == 1 )
        p_dst->p_pixels += p_src->i_pitch;
}
