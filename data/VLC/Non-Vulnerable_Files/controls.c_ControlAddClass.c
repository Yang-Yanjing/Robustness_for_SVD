}
static vlc_v4l2_ctrl_t *ControlAddClass (vlc_object_t *obj, int fd,
                                         const struct v4l2_queryctrl *query)
{
    msg_Dbg (obj, "control class %s:", query->name);
    (void) fd;
    return NULL;
}
