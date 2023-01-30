 */
void vlc_Log (vlc_object_t *obj, int type, const char *module,
              const char *format, ... )
{
    va_list args;
    va_start (args, format);
    vlc_vaLog (obj, type, module, format, args);
    va_end (args);
}
