static av_cold int fbdev_read_close(AVFormatContext *avctx)
{
    FBDevContext *fbdev = avctx->priv_data;
    munmap(fbdev->data, fbdev->fixinfo.smem_len);
    close(fbdev->fd);
    return 0;
}
