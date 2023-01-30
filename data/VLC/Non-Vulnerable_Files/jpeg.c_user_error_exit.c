 */
static void user_error_exit(j_common_ptr p_jpeg)
{
    jpeg_sys_t *p_sys = (jpeg_sys_t *)p_jpeg->err;
    p_sys->err.output_message(p_jpeg);
    longjmp(p_sys->setjmp_buffer, 1);
}
