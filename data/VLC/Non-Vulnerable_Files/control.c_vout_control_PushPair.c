}
void vout_control_PushPair(vout_control_t *ctrl, int type, int a, int b)
{
    vout_control_cmd_t cmd;
    vout_control_cmd_Init(&cmd, type);
    cmd.u.pair.a = a;
    cmd.u.pair.b = b;
    vout_control_Push(ctrl, &cmd);
}
