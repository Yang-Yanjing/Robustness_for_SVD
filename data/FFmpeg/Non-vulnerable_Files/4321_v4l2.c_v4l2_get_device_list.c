static int v4l2_get_device_list(AVFormatContext *ctx, AVDeviceInfoList *device_list)
{
    struct video_data *s = ctx->priv_data;
    DIR *dir;
    struct dirent *entry;
    AVDeviceInfo *device = NULL;
    struct v4l2_capability cap;
    int ret = 0;
    if (!device_list)
        return AVERROR(EINVAL);
    dir = opendir("/dev");
    if (!dir) {
        ret = AVERROR(errno);
        av_log(ctx, AV_LOG_ERROR, "Couldn't open the directory: %s\n", av_err2str(ret));
        return ret;
    }
    while ((entry = readdir(dir))) {
        if (!v4l2_is_v4l_dev(entry->d_name))
            continue;
        snprintf(ctx->filename, sizeof(ctx->filename), "/dev/%s", entry->d_name);
        if ((s->fd = device_open(ctx)) < 0)
            continue;
        if (v4l2_ioctl(s->fd, VIDIOC_QUERYCAP, &cap) < 0) {
            ret = AVERROR(errno);
            av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_QUERYCAP): %s\n", av_err2str(ret));
            goto fail;
        }
        device = av_mallocz(sizeof(AVDeviceInfo));
        if (!device) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        device->device_name = av_strdup(ctx->filename);
        device->device_description = av_strdup(cap.card);
        if (!device->device_name || !device->device_description) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        if ((ret = av_dynarray_add_nofree(&device_list->devices,
                                          &device_list->nb_devices, device)) < 0)
            goto fail;
        v4l2_close(s->fd);
        s->fd = -1;
        continue;
      fail:
        if (device) {
            av_freep(&device->device_name);
            av_freep(&device->device_description);
            av_freep(&device);
        }
        if (s->fd >= 0)
            v4l2_close(s->fd);
        s->fd = -1;
        break;
    }
    closedir(dir);
    return ret;
}
