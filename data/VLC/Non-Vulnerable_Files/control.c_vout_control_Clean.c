}
void vout_control_Clean(vout_control_t *ctrl)
{
    /* */
    for (int i = 0; i < ctrl->cmd.i_size; i++) {
        vout_control_cmd_t cmd = ARRAY_VAL(ctrl->cmd, i);
        vout_control_cmd_Clean(&cmd);
    }
    ARRAY_RESET(ctrl->cmd);
    vlc_mutex_destroy(&ctrl->lock);
    vlc_cond_destroy(&ctrl->wait_request);
    vlc_cond_destroy(&ctrl->wait_acknowledge);
}
