}
static inline void BlendRGBAPixel( picture_t *p_picture,
                                   int i_picture_x, int i_picture_y,
                                   int i_a, int i_r, int i_g, int i_b,
                                   int i_alpha )
{
    int i_an = i_a * i_alpha / 255;
    uint8_t *p_rgba = &p_picture->p->p_pixels[i_picture_y * p_picture->p->i_pitch + 4 * i_picture_x];
    int i_ao = p_rgba[3];
    if( i_ao == 0 )
    {
        p_rgba[0] = i_r;
        p_rgba[1] = i_g;
        p_rgba[2] = i_b;
        p_rgba[3] = i_an;
    }
    else
    {
        p_rgba[3] = 255 - (255 - p_rgba[3]) * (255 - i_an) / 255;
        if( p_rgba[3] != 0 )
        {
            p_rgba[0] = ( p_rgba[0] * i_ao * (255 - i_an) / 255 + i_r * i_an ) / p_rgba[3];
            p_rgba[1] = ( p_rgba[1] * i_ao * (255 - i_an) / 255 + i_g * i_an ) / p_rgba[3];
            p_rgba[2] = ( p_rgba[2] * i_ao * (255 - i_an) / 255 + i_b * i_an ) / p_rgba[3];
        }
    }
}
