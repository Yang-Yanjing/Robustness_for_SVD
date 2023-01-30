 */
static void Close(vout_window_t *wnd)
{
    vout_window_sys_t *p_sys = wnd->sys;
    p_sys->native_window.winRelease(p_sys->window); // Release the native window.
    dlclose(p_sys->p_library); // Close the library.
    free (p_sys);
}
