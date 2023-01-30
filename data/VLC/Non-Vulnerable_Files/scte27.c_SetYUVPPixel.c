}
static inline void SetYUVPPixel(picture_t *picture, int x, int y, int value)
{
    picture->p->p_pixels[y * picture->p->i_pitch + x] = value;
}
