}
void vout_control_PushVoid(vout_control_t *ctrl, int type)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, type);
    vout_control_Push(ctrl, &cmd);
}
