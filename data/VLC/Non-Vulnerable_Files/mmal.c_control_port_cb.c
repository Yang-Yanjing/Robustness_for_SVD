}
static void control_port_cb(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
    vout_display_t *vd = (vout_display_t *)port->userdata;
    MMAL_STATUS_T status;
    if (buffer->cmd == MMAL_EVENT_ERROR) {
        status = *(uint32_t *)buffer->data;
        msg_Err(vd, "MMAL error %"PRIx32" \"%s\"", status, mmal_status_to_string(status));
    }
    mmal_buffer_header_release(buffer);
}
