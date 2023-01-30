#endif /* CAN_COMPILE_SSE2 */
static void CopyPlane(uint8_t *dst, size_t dst_pitch,
                      const uint8_t *src, size_t src_pitch,
                      unsigned width, unsigned height)
{
    for (unsigned y = 0; y < height; y++) {
        memcpy(dst, src, width);
        src += src_pitch;
        dst += dst_pitch;
    }
}
