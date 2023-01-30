}
int screen_CloseCapture(demux_t *p_demux)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    screen_data_t *p_data = p_sys->p_data;
    if (p_data->offscreen_context)
        CFRelease(p_data->offscreen_context);
    if (p_data->offscreen_bitmap)
        free(p_data->offscreen_bitmap);
    if (p_data->p_block)
        block_Release(p_data->p_block);
    free(p_data);
    return VLC_SUCCESS;
}
