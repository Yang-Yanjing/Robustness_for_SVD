    )
static inline void DARKEN_PIXEL(int32_t i_x, int32_t i_y,
    int16_t intensity, plane_t *ps_plane) {
    SUB_MIN( ps_plane->p_pixels[ PIX_OFS(i_x, i_y, ps_plane) ],
        intensity, 0 );
}
