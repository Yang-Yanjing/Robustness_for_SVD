static int first_field(const struct video_data *s)
{
    int res;
    v4l2_std_id std;
    res = v4l2_ioctl(s->fd, VIDIOC_G_STD, &std);
    if (res < 0)
        return 0;
    if (std & V4L2_STD_NTSC)
        return 0;
    return 1;
}
