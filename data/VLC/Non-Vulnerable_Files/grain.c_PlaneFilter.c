}
static void PlaneFilter(filter_t *filter,
                        plane_t *dst, const plane_t *src,
                        int16_t *bank, uint32_t *seed)
{
    filter_sys_t *sys = filter->p_sys;
    for (int y = 0; y < dst->i_visible_lines; y += BLEND_SIZE) {
        for (int x = 0; x < dst->i_visible_pitch; x += BLEND_SIZE) {
            int bx = urand(seed) % (BANK_SIZE - BLEND_SIZE + 1);
            int by = urand(seed) % (BANK_SIZE - BLEND_SIZE + 1);
            const int16_t *noise = &bank[by * BANK_SIZE + bx];
            int w  = dst->i_visible_pitch - x;
            int h  = dst->i_visible_lines - y;
            const uint8_t *srcp = &src->p_pixels[y * src->i_pitch + x];
            uint8_t       *dstp = &dst->p_pixels[y * dst->i_pitch + x];
            if (w >= BLEND_SIZE && h >= BLEND_SIZE)
                sys->blend(dstp, dst->i_pitch, srcp, src->i_pitch, noise);
            else
                BlockBlend(dstp, dst->i_pitch, srcp, src->i_pitch, noise,
                           __MIN(w, BLEND_SIZE), __MIN(h, BLEND_SIZE));
        }
    }
    if (sys->emms)
        sys->emms();
}
}
static void PlaneFilter(filter_t *filter,
                        plane_t *dst, const plane_t *src,
                        int16_t *bank, uint32_t *seed)
{
    filter_sys_t *sys = filter->p_sys;
    for (int y = 0; y < dst->i_visible_lines; y += BLEND_SIZE) {
        for (int x = 0; x < dst->i_visible_pitch; x += BLEND_SIZE) {
            int bx = urand(seed) % (BANK_SIZE - BLEND_SIZE + 1);
            int by = urand(seed) % (BANK_SIZE - BLEND_SIZE + 1);
            const int16_t *noise = &bank[by * BANK_SIZE + bx];
            int w  = dst->i_visible_pitch - x;
            int h  = dst->i_visible_lines - y;
            const uint8_t *srcp = &src->p_pixels[y * src->i_pitch + x];
            uint8_t       *dstp = &dst->p_pixels[y * dst->i_pitch + x];
            if (w >= BLEND_SIZE && h >= BLEND_SIZE)
                sys->blend(dstp, dst->i_pitch, srcp, src->i_pitch, noise);
            else
                BlockBlend(dstp, dst->i_pitch, srcp, src->i_pitch, noise,
                           __MIN(w, BLEND_SIZE), __MIN(h, BLEND_SIZE));
        }
