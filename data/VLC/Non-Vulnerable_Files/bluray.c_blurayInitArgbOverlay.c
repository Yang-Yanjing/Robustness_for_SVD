 */
static void blurayInitArgbOverlay(demux_t *p_demux, int plane, int width, int height)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    blurayInitOverlay(p_demux, plane, width, height);
    if (!p_sys->p_overlays[plane]->p_regions) {
        video_format_t fmt;
        video_format_Init(&fmt, 0);
        video_format_Setup(&fmt, VLC_CODEC_RGBA, width, height, width, height, 1, 1);
        p_sys->p_overlays[plane]->p_regions = subpicture_region_New(&fmt);
    }
}
