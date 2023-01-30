}
static void CreateKateBitmap( picture_t *pic, const kate_bitmap *bitmap )
{
    size_t y;
    for( y=0; y<bitmap->height; ++y )
    {
        uint8_t *dest = pic->Y_PIXELS+pic->Y_PITCH*y;
        const uint8_t *src = bitmap->pixels+y*bitmap->width;
        memcpy( dest, src, bitmap->width );
    }
}
