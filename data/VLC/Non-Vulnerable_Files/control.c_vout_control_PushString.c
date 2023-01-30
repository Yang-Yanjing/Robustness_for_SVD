}
void vout_control_PushString(vout_control_t *ctrl, int type, const char *string)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, type);
    cmd.u.string = strdup(string);
    vout_control_Push(ctrl, &cmd);
}
