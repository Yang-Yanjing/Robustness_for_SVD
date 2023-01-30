#endif
static void default_dds_init( decoder_t * p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* see notes on DDS at the top of the file */
    /* configure for SD res in case DDS is not present */
    p_sys->display.i_version = 0xff; /* an invalid version so it's always different */
    p_sys->display.i_width = 720;
    p_sys->display.i_height = 576;
    p_sys->display.b_windowed = false;
}
