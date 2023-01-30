}
void vout_control_PushInteger(vout_control_t *ctrl, int type, int integer)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, type);
    cmd.u.integer = integer;
    vout_control_Push(ctrl, &cmd);
}
