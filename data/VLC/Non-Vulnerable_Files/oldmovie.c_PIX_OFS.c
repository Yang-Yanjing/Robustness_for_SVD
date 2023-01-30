        ((val+(int32_t)add_val)>max?max:val+add_val)
static inline int32_t PIX_OFS(int32_t i_x, int32_t i_y, plane_t *ps_plane) {
    return i_x * ps_plane->i_pixel_pitch + i_y * ps_plane->i_pitch; }
