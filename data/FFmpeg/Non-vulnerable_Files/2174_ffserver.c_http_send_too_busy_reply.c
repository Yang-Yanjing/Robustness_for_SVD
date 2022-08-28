static void http_send_too_busy_reply(int fd)
{
    char buffer[400];
    int len = snprintf(buffer, sizeof(buffer),
                       "HTTP/1.0 503 Server too busy\r\n"
                       "Content-type: text/html\r\n"
                       "\r\n"
                       "<!DOCTYPE html>\n"
                       "<html><head><title>Too busy</title></head><body>\r\n"
                       "<p>The server is too busy to serve your request at "
                       "this time.</p>\r\n"
                       "<p>The number of current connections is %u, and this "
                       "exceeds the limit of %u.</p>\r\n"
                       "</body></html>\r\n",
                       nb_connections, config.nb_max_connections);
    av_assert0(len < sizeof(buffer));
    if (send(fd, buffer, len, 0) < len)
        av_log(NULL, AV_LOG_WARNING,
               "Could not send too-busy reply, send() failed\n");
}
