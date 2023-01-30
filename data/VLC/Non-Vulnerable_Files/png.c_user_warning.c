}
static void user_warning( png_structp p_png, png_const_charp warning_msg )
{
    png_sys_t *p_sys = (png_sys_t *)png_get_error_ptr( p_png );
    msg_Warn( p_sys->p_obj, "%s", warning_msg );
}
