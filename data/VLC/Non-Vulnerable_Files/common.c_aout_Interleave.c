 */
void aout_Interleave( void *restrict dst, const void *const *srcv,
                      unsigned samples, unsigned chans, vlc_fourcc_t fourcc )
{
#define INTERLEAVE_TYPE(type) \
do { \
    type *d = dst; \
    for( size_t i = 0; i < chans; i++ ) { \
        const type *s = srcv[i]; \
        for( size_t j = 0, k = 0; j < samples; j++, k += chans ) \
            d[k] = *(s++); \
        d++; \
    } \
} while(0)
    switch( fourcc )
    {
        case VLC_CODEC_U8:   INTERLEAVE_TYPE(uint8_t);  break;
        case VLC_CODEC_S16N: INTERLEAVE_TYPE(uint16_t); break;
        case VLC_CODEC_FL32: INTERLEAVE_TYPE(float);    break;
        case VLC_CODEC_S32N: INTERLEAVE_TYPE(int32_t);  break;
        case VLC_CODEC_FL64: INTERLEAVE_TYPE(double);   break;
        default:             assert(0);
    }
#undef INTERLEAVE_TYPE
}
