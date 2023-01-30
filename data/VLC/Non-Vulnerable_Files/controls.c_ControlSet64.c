}
static int ControlSet64 (const vlc_v4l2_ctrl_t *c, int64_t value)
{
    struct v4l2_ext_control ext_ctrl = {
        .id = c->id,
        .size = 0,
    };
    ext_ctrl.value64 = value;
    struct v4l2_ext_controls ext_ctrls = {
        .ctrl_class = V4L2_CTRL_ID2CLASS(c->id),
        .count = 1,
        .error_idx = 0,
        .controls = &ext_ctrl,
    };
    if (v4l2_ioctl (c->fd, VIDIOC_S_EXT_CTRLS, &ext_ctrls) < 0)
        return -1;
    return 0;
}
