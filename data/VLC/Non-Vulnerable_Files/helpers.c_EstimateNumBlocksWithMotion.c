/* See header for function doc. */
int EstimateNumBlocksWithMotion( const picture_t* p_prev,
                                 const picture_t* p_curr,
                                 int *pi_top, int *pi_bot)
{
    assert( p_prev != NULL );
    assert( p_curr != NULL );
    int i_score_top = 0;
    int i_score_bot = 0;
    if( p_prev->i_planes != p_curr->i_planes )
        return -1;
    int (*motion_in_block)(uint8_t *, uint8_t *, int , int, int *, int *) =
        TestForMotionInBlock;
    /* We must tell our inline helper whether to use MMX acceleration. */
#ifdef CAN_COMPILE_MMXEXT
    if (vlc_CPU_MMXEXT())
        motion_in_block = TestForMotionInBlockMMX;
#endif
    int i_score = 0;
    for( int i_plane = 0 ; i_plane < p_prev->i_planes ; i_plane++ )
    {
        /* Sanity check */
        if( p_prev->p[i_plane].i_visible_lines !=
            p_curr->p[i_plane].i_visible_lines )
            return -1;
        const int i_pitch_prev = p_prev->p[i_plane].i_pitch;
        const int i_pitch_curr = p_curr->p[i_plane].i_pitch;
        /* Last pixels and lines (which do not make whole blocks) are ignored.
           Shouldn't really matter for our purposes. */
        const int i_mby = p_prev->p[i_plane].i_visible_lines / 8;
        const int w = FFMIN( p_prev->p[i_plane].i_visible_pitch,
                             p_curr->p[i_plane].i_visible_pitch );
        const int i_mbx = w / 8;
        for( int by = 0; by < i_mby; ++by )
        {
            uint8_t *p_pix_p = &p_prev->p[i_plane].p_pixels[i_pitch_prev*8*by];
            uint8_t *p_pix_c = &p_curr->p[i_plane].p_pixels[i_pitch_curr*8*by];
            for( int bx = 0; bx < i_mbx; ++bx )
            {
                int i_top_temp, i_bot_temp;
                i_score += motion_in_block( p_pix_p, p_pix_c,
                                            i_pitch_prev, i_pitch_curr,
                                            &i_top_temp, &i_bot_temp );
                i_score_top += i_top_temp;
                i_score_bot += i_bot_temp;
                p_pix_p += 8;
                p_pix_c += 8;
            }
        }
    }
    if( pi_top )
        (*pi_top) = i_score_top;
    if( pi_bot )
        (*pi_bot) = i_score_bot;
    return i_score;
}
