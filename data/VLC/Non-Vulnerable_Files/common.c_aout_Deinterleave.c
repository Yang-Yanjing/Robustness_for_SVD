 */
void aout_Deinterleave( void *restrict dst, const void *restrict src,
                      unsigned samples, unsigned chans, vlc_fourcc_t fourcc )
{
#define DEINTERLEAVE_TYPE(type) \
do { \
    type *d = dst; \
    const type *s = src; \
    for( size_t i = 0; i < chans; i++ ) { \
        for( size_t j = 0, k = 0; j < samples; j++, k += chans ) \
            *(d++) = s[k]; \
        s++; \
    } \
} while(0)
    switch( fourcc )
    {
        case VLC_CODEC_U8:   DEINTERLEAVE_TYPE(uint8_t);  break;
        case VLC_CODEC_S16N: DEINTERLEAVE_TYPE(uint16_t); break;
        case VLC_CODEC_FL32: DEINTERLEAVE_TYPE(float);    break;
        case VLC_CODEC_S32N: DEINTERLEAVE_TYPE(int32_t);  break;
        case VLC_CODEC_FL64: DEINTERLEAVE_TYPE(double);   break;
        default:             assert(0);
    }
#undef DEINTERLEAVE_TYPE
}
