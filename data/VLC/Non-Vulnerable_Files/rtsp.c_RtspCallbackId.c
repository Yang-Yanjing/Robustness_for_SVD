/** Non-aggregate RTSP callback */
static int RtspCallbackId( httpd_callback_sys_t *p_args,
                           httpd_client_t *cl,
                           httpd_message_t *answer,
                           const httpd_message_t *query )
{
    rtsp_stream_id_t *id = (rtsp_stream_id_t *)p_args;
    return RtspHandler( id->stream, id, cl, answer, query );
}
