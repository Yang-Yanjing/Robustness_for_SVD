}
static void Manage (vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    CommonManage(vd);
    const int width  = sys->rect_dest.right  - sys->rect_dest.left;
    const int height = sys->rect_dest.bottom - sys->rect_dest.top;
    glViewport(0, 0, width, height);
}
