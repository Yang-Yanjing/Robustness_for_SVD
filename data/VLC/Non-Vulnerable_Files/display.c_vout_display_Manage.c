}
static void vout_display_Manage(vout_display_t *vd)
{
    if (vd->manage)
        vd->manage(vd);
}
