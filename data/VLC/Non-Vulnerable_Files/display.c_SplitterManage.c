}
static void SplitterManage(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    for (int i = 0; i < sys->count; i++)
        vout_ManageDisplay(sys->display[i], true);
}
