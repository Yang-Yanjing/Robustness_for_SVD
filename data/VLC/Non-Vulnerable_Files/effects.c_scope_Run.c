 *****************************************************************************/
static int scope_Run(visual_effect_t * p_effect, vlc_object_t *p_aout,
                     const block_t * p_buffer , picture_t * p_picture)
{
    VLC_UNUSED(p_aout);
    int i_index;
    float *p_sample ;
    uint8_t *ppp_area[2][3];
    for( i_index = 0 ; i_index < 2 ; i_index++ )
    {
        for( int j = 0 ; j < 3 ; j++ )
        {
            ppp_area[i_index][j] =
                p_picture->p[j].p_pixels + (i_index * 2 + 1) * p_picture->p[j].i_lines
                / 4 * p_picture->p[j].i_pitch;
        }
    }
    for( i_index = 0, p_sample = (float *)p_buffer->p_buffer;
            i_index < __MIN( p_effect->i_width, (int)p_buffer->i_nb_samples );
            i_index++ )
    {
        int8_t i_value;
        /* Left channel */
        i_value =  p_sample[p_effect->i_idx_left] * 127;
        *(ppp_area[0][0]
                + p_picture->p[0].i_pitch * i_index / p_effect->i_width
                + p_picture->p[0].i_lines * i_value / 512
                * p_picture->p[0].i_pitch) = 0xbf;
        *(ppp_area[0][1]
                + p_picture->p[1].i_pitch * i_index / p_effect->i_width
                + p_picture->p[1].i_lines * i_value / 512
                * p_picture->p[1].i_pitch) = 0xff;
        /* Right channel */
        i_value = p_sample[p_effect->i_idx_right] * 127;
        *(ppp_area[1][0]
                + p_picture->p[0].i_pitch * i_index / p_effect->i_width
                + p_picture->p[0].i_lines * i_value / 512
                * p_picture->p[0].i_pitch) = 0x9f;
        *(ppp_area[1][2]
                + p_picture->p[2].i_pitch * i_index / p_effect->i_width
                + p_picture->p[2].i_lines * i_value / 512
                * p_picture->p[2].i_pitch) = 0xdd;
        p_sample += p_effect->i_nb_chans;
    }
    return 0;
}
