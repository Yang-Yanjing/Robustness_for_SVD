}
static void fetch_trigo( struct filter_sys_t *sys, int *i_sin, int *i_cos )
{
    uint32_t sincos = atomic_load( &sys->sincos );
    *i_sin = (int16_t)(sincos & 0xFFFF);
    *i_cos = (int16_t)(sincos >> 16);
}
