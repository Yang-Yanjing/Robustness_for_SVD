void vout_control_cmd_Clean(vout_control_cmd_t *cmd)
{
    switch (cmd->type) {
    case VOUT_CONTROL_SUBPICTURE:
        if (cmd->u.subpicture)
            subpicture_Delete(cmd->u.subpicture);
        break;
    case VOUT_CONTROL_OSD_TITLE:
    case VOUT_CONTROL_CHANGE_FILTERS:
    case VOUT_CONTROL_CHANGE_SUB_SOURCES:
    case VOUT_CONTROL_CHANGE_SUB_FILTERS:
        free(cmd->u.string);
        break;
    default:
        break;
    }
}
