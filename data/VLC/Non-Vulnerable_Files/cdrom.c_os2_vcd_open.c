 *****************************************************************************/
static int os2_vcd_open( vlc_object_t * p_this, const char *psz_dev,
                         vcddev_t *p_vcddev )
{
    char device[] = "X:";
    HFILE hcd;
    ULONG i_action;
    ULONG rc;
    p_vcddev->hcd = 0;
    device[0] = psz_dev[0];
    rc = DosOpen( device, &hcd, &i_action, 0, FILE_NORMAL,
                  OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
                  OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE | OPEN_FLAGS_DASD,
                  NULL);
    if( rc )
    {
        msg_Err( p_this, "could not open the device %s", psz_dev );
        return -1;
    }
    p_vcddev->hcd = hcd;
    return 0;
}
