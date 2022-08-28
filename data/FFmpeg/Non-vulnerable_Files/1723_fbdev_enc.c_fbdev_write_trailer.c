static av_cold int fbdev_write_trailer(AVFormatContext *h)
{
    FBDevContext *fbdev = h->priv_data;
    munmap(fbdev->data, fbdev->fixinfo.smem_len);
    close(fbdev->fd);
    return 0;
}
