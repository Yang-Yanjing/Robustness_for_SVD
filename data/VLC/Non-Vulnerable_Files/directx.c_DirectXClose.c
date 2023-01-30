}
static void DirectXClose(vout_display_t *vd)
{
    DirectXDestroyPool(vd);
    DirectXCloseDisplay(vd);
    DirectXCloseDDraw(vd);
}
