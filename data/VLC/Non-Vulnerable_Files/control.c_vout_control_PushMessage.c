}
void vout_control_PushMessage(vout_control_t *ctrl, int type, int channel, const char *string)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, type);
    cmd.u.message.channel = channel;
    cmd.u.message.string = strdup(string);
    vout_control_Push(ctrl, &cmd);
}
