}
void vout_control_PushTime(vout_control_t *ctrl, int type, mtime_t time)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, type);
    cmd.u.time = time;
    vout_control_Push(ctrl, &cmd);
}
