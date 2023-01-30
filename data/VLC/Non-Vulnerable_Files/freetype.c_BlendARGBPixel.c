}
static inline void BlendARGBPixel(picture_t *pic, int pic_x, int pic_y,
                                  int a, int r, int g, int b, int alpha)
{
    uint8_t *rgba = &pic->p->p_pixels[pic_y * pic->p->i_pitch + 4 * pic_x];
    int an = a * alpha / 255;
    int ao = rgba[3];
    if (ao == 0)
    {
        rgba[0] = an;
        rgba[1] = r;
        rgba[2] = g;
        rgba[3] = b;
    }
    else
    {
        rgba[0] = 255 - (255 - rgba[0]) * (255 - an) / 255;
        if (rgba[0] != 0)
        {
            rgba[1] = (rgba[1] * ao * (255 - an) / 255 + r * an ) / rgba[0];
            rgba[2] = (rgba[2] * ao * (255 - an) / 255 + g * an ) / rgba[0];
            rgba[3] = (rgba[3] * ao * (255 - an) / 255 + b * an ) / rgba[0];
        }
    }
}
