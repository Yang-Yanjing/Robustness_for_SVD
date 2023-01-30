 */
void aout_ChannelReorder( void *ptr, size_t bytes, unsigned channels,
                          const uint8_t *restrict chans_table, vlc_fourcc_t fourcc )
{
    assert( channels != 0 );
    assert( channels <= AOUT_CHAN_MAX );
    /* The audio formats supported in audio output are inlined. For other
     * formats (used in demuxers and muxers), memcpy() is used to avoid
     * breaking type punning. */
#define REORDER_TYPE(type) \
do { \
    const size_t frames = (bytes / sizeof (type)) / channels; \
    type *buf = ptr; \
\
    for( size_t i = 0; i < frames; i++ ) \
    { \
        type tmp[AOUT_CHAN_MAX]; \
\
        for( size_t j = 0; j < channels; j++ ) \
            tmp[chans_table[j]] = buf[j]; \
        memcpy( buf, tmp, sizeof (type) * channels ); \
        buf += channels; \
    } \
} while(0)
    switch( fourcc )
    {
        case VLC_CODEC_U8:   REORDER_TYPE(uint8_t); break;
        case VLC_CODEC_S16N: REORDER_TYPE(int16_t); break;
        case VLC_CODEC_FL32: REORDER_TYPE(float);   break;
        case VLC_CODEC_S32N: REORDER_TYPE(int32_t); break;
        case VLC_CODEC_FL64: REORDER_TYPE(double);  break;
        default:
        {
            unsigned size = aout_BitsPerSample( fourcc ) / 8;
            assert( size != 0 );
            const size_t frames = bytes / (size * channels);
            unsigned char *buf = ptr;
            assert( bytes != 0 );
            for( size_t i = 0; i < frames; i++ )
            {
                unsigned char tmp[AOUT_CHAN_MAX * size];
                for( size_t j = 0; j < channels; j++ )
                    memcpy( tmp + size * chans_table[j], buf + size * j, size );
                memcpy( buf, tmp, size * channels );
                buf += size * channels;
            }
            break;
        }
    }
}
