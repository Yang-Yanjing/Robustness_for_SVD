static void calculate_display_rect(SDL_Rect *rect,
                                   int scr_xleft, int scr_ytop, int scr_width, int scr_height,
                                   int pic_width, int pic_height, AVRational pic_sar)
{
    float aspect_ratio;
    int width, height, x, y;
    if (pic_sar.num == 0)
        aspect_ratio = 0;
    else
        aspect_ratio = av_q2d(pic_sar);
    if (aspect_ratio <= 0.0)
        aspect_ratio = 1.0;
    aspect_ratio *= (float)pic_width / (float)pic_height;
    
    height = scr_height;
    width = lrint(height * aspect_ratio) & ~1;
    if (width > scr_width) {
        width = scr_width;
        height = lrint(width / aspect_ratio) & ~1;
    }
    x = (scr_width - width) / 2;
    y = (scr_height - height) / 2;
    rect->x = scr_xleft + x;
    rect->y = scr_ytop  + y;
    rect->w = FFMAX(width,  1);
    rect->h = FFMAX(height, 1);
}
