}
static inline void BlendYUVAPixel( picture_t *p_picture,
                                   int i_picture_x, int i_picture_y,
                                   int i_a, int i_y, int i_u, int i_v,
                                   int i_alpha )
{
    int i_an = i_a * i_alpha / 255;
    uint8_t *p_y = &p_picture->p[0].p_pixels[i_picture_y * p_picture->p[0].i_pitch + i_picture_x];
    uint8_t *p_u = &p_picture->p[1].p_pixels[i_picture_y * p_picture->p[1].i_pitch + i_picture_x];
    uint8_t *p_v = &p_picture->p[2].p_pixels[i_picture_y * p_picture->p[2].i_pitch + i_picture_x];
    uint8_t *p_a = &p_picture->p[3].p_pixels[i_picture_y * p_picture->p[3].i_pitch + i_picture_x];
    int i_ao = *p_a;
    if( i_ao == 0 )
    {
        *p_y = i_y;
        *p_u = i_u;
        *p_v = i_v;
        *p_a = i_an;
    }
    else
    {
        *p_a = 255 - (255 - *p_a) * (255 - i_an) / 255;
        if( *p_a != 0 )
        {
            *p_y = ( *p_y * i_ao * (255 - i_an) / 255 + i_y * i_an ) / *p_a;
            *p_u = ( *p_u * i_ao * (255 - i_an) / 255 + i_u * i_an ) / *p_a;
            *p_v = ( *p_v * i_ao * (255 - i_an) / 255 + i_v * i_an ) / *p_a;
        }
    }
}
