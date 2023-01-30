 */
static int Open (vlc_object_t *obj)
{
    services_discovery_t *sd = (services_discovery_t *)obj;
    HFILE hcd2;
    ULONG ulAction;
    ULONG ulParamLen;
    ULONG ulData;
    ULONG ulDataLen;
    ULONG rc;
    if (DosOpen ((PSZ)"CD-ROM2$", (PHFILE)&hcd2, &ulAction, 0, FILE_NORMAL,
                 OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
                 OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL))
        return VLC_EGENERIC;
    rc = DosDevIOCtl (hcd2, IOCTL_CDROMDISK2, CDROMDISK2_DRIVELETTERS,
                      NULL, 0, &ulParamLen, &ulData, sizeof(ulData), &ulDataLen);
    if (!rc)
    {
        char mrl[] = "file:///A:/", name[] = "A:";
        int count = LOUSHORT(ulData);
        int drive = HIUSHORT(ulData);
        input_item_t *item;
        char          letter;
        for (; count; --count, ++drive)
        {
            letter = 'A' + drive;
            mrl[8] = name[0] = letter;
            item = input_item_NewWithType (mrl, name, 0, NULL, 0, -1, ITEM_TYPE_DISC);
            msg_Dbg (sd, "adding %s (%s)", mrl, name);
            if (item == NULL)
                break;
            services_discovery_AddItem (sd, item, _("Local drives"));
        }
    }
    DosClose (hcd2);
    return rc ? VLC_EGENERIC : VLC_SUCCESS;
}
