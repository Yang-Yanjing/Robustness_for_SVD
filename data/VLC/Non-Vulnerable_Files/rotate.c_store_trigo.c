};
static void store_trigo( struct filter_sys_t *sys, float f_angle )
{
    f_angle *= M_PI / 180.f; /* degrees -> radians */
    uint16_t i_sin = lroundf(sinf(f_angle) * 4096.f);
    uint16_t i_cos = lroundf(cosf(f_angle) * 4096.f);
    atomic_store( &sys->sincos, (i_cos << 16) | (i_sin << 0));
}
