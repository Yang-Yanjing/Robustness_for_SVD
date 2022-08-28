static av_always_inline int dither_color(uint32_t px, int er, int eg, int eb, int scale, int shift)
{
    return av_clip_uint8((px >> 16 & 0xff) + ((er * scale) / (1<<shift))) << 16
         | av_clip_uint8((px >>  8 & 0xff) + ((eg * scale) / (1<<shift))) <<  8
         | av_clip_uint8((px       & 0xff) + ((eb * scale) / (1<<shift)));
}
