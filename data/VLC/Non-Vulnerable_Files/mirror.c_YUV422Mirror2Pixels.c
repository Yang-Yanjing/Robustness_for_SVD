 *****************************************************************************/
static void YUV422Mirror2Pixels( uint8_t* p_dst, uint8_t *p_src,
                                bool b_y_is_odd )
{
    if ( b_y_is_odd )
    {
        /* swap Y components */
        *p_dst = *( p_src + 2 );
        *( p_dst + 2 ) = *p_src;
        /* copy Cb and Cr components */
        *( p_dst + 1 ) = *( p_src + 1 );
        *( p_dst + 3 ) = *( p_src + 3 );
    }
    else{
        /* swap Y components */
        *( p_dst + 1 )= *( p_src + 3 );
        *( p_dst + 3 ) = *( p_src + 1);
        /* copy Cb and Cr components */
        *p_dst = *( p_src + 2 );
        *( p_dst + 2 ) = *p_src;
    }
}
