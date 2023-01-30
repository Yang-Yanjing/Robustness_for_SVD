 */
static void user_error_message(j_common_ptr p_jpeg)
{
    char error_msg[JMSG_LENGTH_MAX];
    jpeg_sys_t *p_sys = (jpeg_sys_t *)p_jpeg->err;
    p_sys->err.format_message(p_jpeg, error_msg);
    msg_Err(p_sys->p_obj, "%s", error_msg);
}
