}
static void FillARGBPicture(picture_t *pic, int a, int r, int g, int b)
{
    if (a == 0)
        r = g = b = 0;
    if (a == r && a == b && a == g)
    {   /* fast path */
        memset(pic->p->p_pixels, a, pic->p->i_visible_lines * pic->p->i_pitch);
        return;
    }
    uint_fast32_t pixel = VLC_FOURCC(a, r, g, b);
    uint8_t *line = pic->p->p_pixels;
    for (unsigned lines = pic->p->i_visible_lines; lines > 0; lines--)
    {
        uint32_t *pixels = (uint32_t *)line;
        for (unsigned cols = pic->p->i_visible_pitch; cols > 0; cols -= 4)
            *(pixels++) = pixel;
        line += pic->p->i_pitch;
    }
}
