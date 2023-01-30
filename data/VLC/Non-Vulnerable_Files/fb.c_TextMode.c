 *****************************************************************************/
static void TextMode(int tty)
{
    /* return to text mode */
    if (-1 == ioctl(tty, KDSETMODE, KD_TEXT)) {
        /*msg_Err(vd, "failed ioctl KDSETMODE KD_TEXT");*/
    }
}
