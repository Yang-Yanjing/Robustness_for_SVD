 *****************************************************************************/
vout_display_t *vout_NewDisplay(vout_thread_t *vout,
                                const video_format_t *source,
                                const vout_display_state_t *state,
                                const char *module,
                                mtime_t double_click_timeout,
                                mtime_t hide_timeout)
{
    return DisplayNew(vout, source, state, module, false, NULL,
                      double_click_timeout, hide_timeout, NULL);
}
