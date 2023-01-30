 */
int XCB_picture_Alloc (vout_display_t *vd, picture_resource_t *res,
                       size_t size, xcb_connection_t *conn,
                       xcb_shm_seg_t segment)
{
#ifdef HAVE_SYS_SHM_H
    /* Allocate shared memory segment */
    int id = shmget (IPC_PRIVATE, size, IPC_CREAT | S_IRWXU);
    if (id == -1)
    {
        msg_Err (vd, "shared memory allocation error: %s",
                 vlc_strerror_c(errno));
        return -1;
    }
    /* Attach the segment to VLC */
    void *shm = shmat (id, NULL, 0 /* read/write */);
    if (-1 == (intptr_t)shm)
    {
        msg_Err (vd, "shared memory attachment error: %s",
                 vlc_strerror_c(errno));
        shmctl (id, IPC_RMID, 0);
        return -1;
    }
    if (segment != 0)
    {   /* Attach the segment to X */
        xcb_void_cookie_t ck = xcb_shm_attach_checked (conn, segment, id, 1);
        switch (XCB_error_Check (vd, conn, "shared memory server-side error",
                                 ck))
        {
            case 0:
                break;
            case XCB_ACCESS:
            {
                struct shmid_ds buf;
                /* Retry with promiscuous permissions */
                shmctl (id, IPC_STAT, &buf);
                buf.shm_perm.mode |= S_IRGRP|S_IROTH;
                shmctl (id, IPC_SET, &buf);
                ck = xcb_shm_attach_checked (conn, segment, id, 1);
                if (XCB_error_Check (vd, conn, "same error on retry", ck) == 0)
                    break;
                /* fall through */
            }
            default:
                msg_Info (vd, "using buggy X11 server - SSH proxying?");
                segment = 0;
        }
    }
    shmctl (id, IPC_RMID, NULL);
#else
    assert (segment == 0);
    /* XXX: align on 32 bytes for VLC chroma filters */
    void *shm = malloc (size);
    if (unlikely(shm == NULL))
        return -1;
#endif
    res->p_sys = (void *)(uintptr_t)segment;
    res->pf_destroy = XCB_picture_Destroy;
    res->p[0].p_pixels = shm;
    return 0;
}
