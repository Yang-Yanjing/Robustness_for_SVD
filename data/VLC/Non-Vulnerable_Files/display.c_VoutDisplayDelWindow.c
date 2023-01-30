}
static void VoutDisplayDelWindow(vout_display_t *vd, vout_window_t *window)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
#ifdef ALLOW_DUMMY_VOUT
    if (!osys->vout->p) {
        if( window)
            vout_window_Delete(window);
        return;
    }
#endif
    vout_DeleteDisplayWindow(osys->vout, vd, window);
}
