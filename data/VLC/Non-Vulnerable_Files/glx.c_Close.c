 */
static void Close (vlc_object_t *obj)
{
    vout_display_t *vd = (vout_display_t *)obj;
    vout_display_sys_t *sys = vd->sys;
    vlc_gl_MakeCurrent (sys->gl);
    vout_display_opengl_Delete (sys->vgl);
    vlc_gl_ReleaseCurrent (sys->gl);
    vlc_gl_Destroy (sys->gl);
    /* show the default cursor */
    xcb_change_window_attributes (sys->conn, sys->embed->handle.xid,
                               XCB_CW_CURSOR, &(uint32_t) { XCB_CURSOR_NONE });
    xcb_flush (sys->conn);
    xcb_disconnect (sys->conn);
    vout_display_DeleteWindow (vd, sys->embed);
    free (sys);
}
