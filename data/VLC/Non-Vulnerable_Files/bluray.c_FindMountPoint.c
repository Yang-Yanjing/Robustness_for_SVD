/* */
static void FindMountPoint(char **file)
{
    char *device = *file;
#if defined (HAVE_MNTENT_H) && defined (HAVE_SYS_STAT_H)
    struct stat st;
    if (!stat (device, &st) && S_ISBLK (st.st_mode)) {
        FILE *mtab = setmntent ("/proc/self/mounts", "r");
        struct mntent *m, mbuf;
        char buf [8192];
        /* bd path may be a symlink (e.g. /dev/dvd -> /dev/sr0), so make
         * sure we look up the real device */
        char *bd_device = realpath(device, NULL);
        if (!bd_device)
            bd_device = strdup(device);
        while ((m = getmntent_r (mtab, &mbuf, buf, sizeof(buf))) != NULL) {
            if (!strcmp (m->mnt_fsname, bd_device)) {
                free(device);
                *file = strdup(m->mnt_dir);
                break;
            }
        }
        free(bd_device);
        endmntent (mtab);
    }
#elif defined(__APPLE__)
    struct stat st;
    if (!stat (device, &st) && S_ISBLK (st.st_mode)) {
        int fs_count = getfsstat (NULL, 0, MNT_NOWAIT);
        if (fs_count > 0) {
            struct statfs mbuf[128];
            getfsstat (mbuf, fs_count * sizeof(mbuf[0]), MNT_NOWAIT);
            for (int i = 0; i < fs_count; ++i)
                if (!strcmp (mbuf[i].f_mntfromname, device)) {
                    free(device);
                    *file = strdup(mbuf[i].f_mntonname);
                    return;
                }
        }
    }
#else
# warning Disc device to mount point not implemented
#endif
}
