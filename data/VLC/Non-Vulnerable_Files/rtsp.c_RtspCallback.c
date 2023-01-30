/** Aggregate RTSP callback */
static int RtspCallback( httpd_callback_sys_t *p_args,
                         httpd_client_t *cl,
                         httpd_message_t *answer,
                         const httpd_message_t *query )
{
    return RtspHandler( (rtsp_stream_t *)p_args, NULL, cl, answer, query );
}
