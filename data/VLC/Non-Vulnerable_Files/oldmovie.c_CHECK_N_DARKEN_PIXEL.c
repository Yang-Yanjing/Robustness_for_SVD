}
static inline void CHECK_N_DARKEN_PIXEL(int32_t i_x, int32_t i_y,
                                int16_t intensity, plane_t *ps_plane) {
    if ( likely( CHECK_PIX_OFS(i_x, i_y, ps_plane) ) )
        DARKEN_PIXEL(i_x, i_y, intensity, ps_plane);
}
