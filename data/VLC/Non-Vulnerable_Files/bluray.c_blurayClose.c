 *****************************************************************************/
static void blurayClose(vlc_object_t *object)
{
    demux_t *p_demux = (demux_t*)object;
    demux_sys_t *p_sys = p_demux->p_sys;
    setTitleInfo(p_sys, NULL);
    /*
     * Close libbluray first.
     * This will close all the overlays before we release p_vout
     * bd_close(NULL) can crash
     */
    assert(p_sys->bluray);
    bd_close(p_sys->bluray);
    if (p_sys->p_vout != NULL) {
        var_DelCallback(p_sys->p_vout, "mouse-moved", onMouseEvent, p_demux);
        var_DelCallback(p_sys->p_vout, "mouse-clicked", onMouseEvent, p_demux);
        vlc_object_release(p_sys->p_vout);
    }
    if (p_sys->p_input != NULL)
        vlc_object_release(p_sys->p_input);
    if (p_sys->p_parser)
        stream_Delete(p_sys->p_parser);
    if (p_sys->p_out != NULL)
        es_out_Delete(p_sys->p_out);
    assert(vlc_array_count(&p_sys->es) == 0);
    vlc_array_clear(&p_sys->es);
    /* Titles */
    for (unsigned int i = 0; i < p_sys->i_title; i++)
        vlc_input_title_Delete(p_sys->pp_title[i]);
    TAB_CLEAN(p_sys->i_title, p_sys->pp_title);
    vlc_mutex_destroy(&p_sys->pl_info_lock);
    free(p_sys->psz_bd_path);
    free(p_sys);
}
