}
static CGContextRef CreateOffScreenContext(int i_width, int i_height,
                         offscreen_bitmap_t **pp_memory, CGColorSpaceRef *pp_colorSpace)
{
    offscreen_bitmap_t *p_bitmap;
    CGContextRef        p_context = NULL;
    p_bitmap = (offscreen_bitmap_t *) malloc(sizeof(offscreen_bitmap_t));
    if (p_bitmap) {
        p_bitmap->i_bitsPerChannel = 8;
        p_bitmap->i_bitsPerPixel   = 4 * p_bitmap->i_bitsPerChannel; // A,R,G,B
        p_bitmap->i_bytesPerPixel  = p_bitmap->i_bitsPerPixel / 8;
        p_bitmap->i_bytesPerRow    = i_width * p_bitmap->i_bytesPerPixel;
        p_bitmap->p_data = calloc(i_height, p_bitmap->i_bytesPerRow);
        *pp_colorSpace = CGColorSpaceCreateDeviceRGB();
        if (p_bitmap->p_data && *pp_colorSpace)
            p_context = CGBitmapContextCreate(p_bitmap->p_data, i_width, i_height,
                                p_bitmap->i_bitsPerChannel, p_bitmap->i_bytesPerRow,
                                *pp_colorSpace, kCGImageAlphaPremultipliedFirst);
        if (p_context) {
            if (&CGContextSetAllowsAntialiasing != NULL)
                CGContextSetAllowsAntialiasing(p_context, true);
        }
        *pp_memory = p_bitmap;
    }
    return p_context;
}
