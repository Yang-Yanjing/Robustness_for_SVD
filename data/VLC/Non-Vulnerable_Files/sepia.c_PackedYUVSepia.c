 *****************************************************************************/
static void PackedYUVSepia( picture_t *p_pic, picture_t *p_outpic,
                           int i_intensity )
{
    uint8_t *p_in, *p_in_end, *p_line_end, *p_out;
    int i_yindex = 1, i_uindex = 2, i_vindex = 0;
    GetPackedYuvOffsets( p_outpic->format.i_chroma,
                        &i_yindex, &i_uindex, &i_vindex );
    // prepared values to copy for U and V channels
    const uint8_t filling_const_8u = 128 - i_intensity / 6;
    const uint8_t filling_const_8v = 128 + i_intensity / 14;
    p_in = p_pic->p[0].p_pixels;
    p_in_end = p_in + p_pic->p[0].i_visible_lines
        * p_pic->p[0].i_pitch;
    p_out = p_outpic->p[0].p_pixels;
    {
        while( p_in < p_in_end )
        {
            p_line_end = p_in + p_pic->p[0].i_visible_pitch;
            while( p_in < p_line_end )
            {
                /* calculate new, sepia values */
                p_out[i_yindex] =
                    p_in[i_yindex] - (p_in[i_yindex] >> 2) + (i_intensity >> 2);
                p_out[i_yindex + 2] =
                    p_in[i_yindex + 2] - (p_in[i_yindex + 2] >> 2)
                    + (i_intensity >> 2);
                p_out[i_uindex] = filling_const_8u;
                p_out[i_vindex] = filling_const_8v;
                p_in += 4;
                p_out += 4;
            }
            p_in += p_pic->p[0].i_pitch - p_pic->p[0].i_visible_pitch;
            p_out += p_outpic->p[0].i_pitch
                - p_outpic->p[0].i_visible_pitch;
        }
    }
}
