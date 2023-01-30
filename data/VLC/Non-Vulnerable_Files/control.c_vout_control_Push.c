}
void vout_control_Push(vout_control_t *ctrl, vout_control_cmd_t *cmd)
{
    vlc_mutex_lock(&ctrl->lock);
    if (!ctrl->is_dead) {
        ARRAY_APPEND(ctrl->cmd, *cmd);
        vlc_cond_signal(&ctrl->wait_request);
    } else {
        vout_control_cmd_Clean(cmd);
    }
    vlc_mutex_unlock(&ctrl->lock);
}
