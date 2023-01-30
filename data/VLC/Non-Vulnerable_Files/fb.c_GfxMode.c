}
static void GfxMode(int tty)
{
    /* switch to graphic mode */
    if (-1 == ioctl(tty, KDSETMODE, KD_GRAPHICS)) {
        /*msg_Err(vd, "failed ioctl KDSETMODE KD_GRAPHICS");*/
    }
}
