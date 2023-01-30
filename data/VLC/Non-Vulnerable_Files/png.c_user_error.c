}
static void user_error( png_structp p_png, png_const_charp error_msg )
{
    png_sys_t *p_sys = (png_sys_t *)png_get_error_ptr( p_png );
    p_sys->b_error = true;
    msg_Err( p_sys->p_obj, "%s", error_msg );
}
