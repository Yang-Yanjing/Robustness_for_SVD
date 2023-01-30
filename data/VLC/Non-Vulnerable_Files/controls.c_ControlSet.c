};
static int ControlSet (const vlc_v4l2_ctrl_t *c, int_fast32_t value)
{
    struct v4l2_control ctrl = {
        .id = c->id,
        .value = value,
    };
    if (v4l2_ioctl (c->fd, VIDIOC_S_CTRL, &ctrl) < 0)
        return -1;
    return 0;
}
