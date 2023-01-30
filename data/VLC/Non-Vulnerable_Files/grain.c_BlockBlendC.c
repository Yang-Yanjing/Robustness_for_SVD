}
static void BlockBlendC(uint8_t *dst, size_t dst_pitch,
                        const uint8_t *src, size_t src_pitch,
                        const int16_t *noise)
{
    BlockBlend(dst, dst_pitch, src, src_pitch, noise,
               BLEND_SIZE, BLEND_SIZE);
}
