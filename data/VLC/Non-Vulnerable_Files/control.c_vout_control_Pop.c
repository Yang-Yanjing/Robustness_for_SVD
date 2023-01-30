}
int vout_control_Pop(vout_control_t *ctrl, vout_control_cmd_t *cmd,
                     mtime_t deadline, mtime_t timeout)
{
    vlc_mutex_lock(&ctrl->lock);
    if (ctrl->cmd.i_size <= 0) {
        ctrl->is_processing = false;
        vlc_cond_broadcast(&ctrl->wait_acknowledge);
        const mtime_t max_deadline = mdate() + timeout;
        const mtime_t wait_deadline = deadline <= VLC_TS_INVALID ? max_deadline : __MIN(deadline, max_deadline);
        /* Spurious wakeups are perfectly fine */
        if (ctrl->can_sleep)
            vlc_cond_timedwait(&ctrl->wait_request, &ctrl->lock, wait_deadline);
    }
    bool has_cmd;
    if (ctrl->cmd.i_size > 0) {
        has_cmd = true;
        *cmd = ARRAY_VAL(ctrl->cmd, 0);
        ARRAY_REMOVE(ctrl->cmd, 0);
        ctrl->is_processing = true;
    } else {
        has_cmd = false;
        ctrl->can_sleep = true;
    }
    vlc_mutex_unlock(&ctrl->lock);
    return has_cmd ? VLC_SUCCESS : VLC_EGENERIC;
}
