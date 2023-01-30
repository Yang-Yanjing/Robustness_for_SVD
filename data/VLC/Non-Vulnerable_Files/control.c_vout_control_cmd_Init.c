/* */
void vout_control_cmd_Init(vout_control_cmd_t *cmd, int type)
{
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = type;
}
