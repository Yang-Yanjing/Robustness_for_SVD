 */
static int vout_display_Control(vout_display_t *vd, int query, ...)
{
    va_list args;
    int result;
    va_start(args, query);
    result = vd->control(vd, query, args);
    va_end(args);
    return result;
}
