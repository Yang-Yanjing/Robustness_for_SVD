}
void vout_control_PushBool(vout_control_t *ctrl, int type, bool boolean)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, type);
    cmd.u.boolean = boolean;
    vout_control_Push(ctrl, &cmd);
}
