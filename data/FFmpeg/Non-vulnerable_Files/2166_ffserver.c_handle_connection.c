static int handle_connection(HTTPContext *c)
{
    int len, ret;
    uint8_t *ptr;
    switch(c->state) {
    case HTTPSTATE_WAIT_REQUEST:
    case RTSPSTATE_WAIT_REQUEST:
        
        if ((c->timeout - cur_time) < 0)
            return -1;
        if (c->poll_entry->revents & (POLLERR | POLLHUP))
            return -1;
        
        if (!(c->poll_entry->revents & POLLIN))
            return 0;
        
    read_loop:
        if (!(len = recv(c->fd, c->buffer_ptr, 1, 0)))
            return -1;
        if (len < 0) {
            if (ff_neterrno() != AVERROR(EAGAIN) &&
                ff_neterrno() != AVERROR(EINTR))
                return -1;
            break;
        }
        
        c->buffer_ptr += len;
        ptr = c->buffer_ptr;
        if ((ptr >= c->buffer + 2 && !memcmp(ptr-2, "\n\n", 2)) ||
            (ptr >= c->buffer + 4 && !memcmp(ptr-4, "\r\n\r\n", 4))) {
            
            if (c->state == HTTPSTATE_WAIT_REQUEST)
                ret = http_parse_request(c);
            else
                ret = rtsp_parse_request(c);
            if (ret < 0)
                return -1;
        } else if (ptr >= c->buffer_end) {
            
            return -1;
        } else goto read_loop;
        break;
    case HTTPSTATE_SEND_HEADER:
        if (c->poll_entry->revents & (POLLERR | POLLHUP))
            return -1;
        
        if (!(c->poll_entry->revents & POLLOUT))
            return 0;
        len = send(c->fd, c->buffer_ptr, c->buffer_end - c->buffer_ptr, 0);
        if (len < 0) {
            if (ff_neterrno() != AVERROR(EAGAIN) &&
                ff_neterrno() != AVERROR(EINTR)) {
                goto close_connection;
            }
            break;
        }
        c->buffer_ptr += len;
        if (c->stream)
            c->stream->bytes_served += len;
        c->data_count += len;
        if (c->buffer_ptr >= c->buffer_end) {
            av_freep(&c->pb_buffer);
            
            if (c->http_error)
                return -1;
            

            c->state = HTTPSTATE_SEND_DATA_HEADER;
            c->buffer_ptr = c->buffer_end = c->buffer;
        }
        break;
    case HTTPSTATE_SEND_DATA:
    case HTTPSTATE_SEND_DATA_HEADER:
    case HTTPSTATE_SEND_DATA_TRAILER:
        


        if (!c->is_packetized) {
            if (c->poll_entry->revents & (POLLERR | POLLHUP))
                return -1;
            
            if (!(c->poll_entry->revents & POLLOUT))
                return 0;
        }
        if (http_send_data(c) < 0)
            return -1;
        
        if (c->state == HTTPSTATE_SEND_DATA_TRAILER)
            return -1;
        
        if (c->stream->single_frame && c->data_count > c->cur_frame_bytes && c->cur_frame_bytes > 0) {
            close_connection(c);
        }
        break;
    case HTTPSTATE_RECEIVE_DATA:
        
        if (c->poll_entry->revents & (POLLERR | POLLHUP))
            return -1;
        if (!(c->poll_entry->revents & POLLIN))
            return 0;
        if (http_receive_data(c) < 0)
            return -1;
        break;
    case HTTPSTATE_WAIT_FEED:
        
        if (c->poll_entry->revents & (POLLIN | POLLERR | POLLHUP))
            return -1;
        
        break;
    case RTSPSTATE_SEND_REPLY:
        if (c->poll_entry->revents & (POLLERR | POLLHUP))
            goto close_connection;
        
        if (!(c->poll_entry->revents & POLLOUT))
            return 0;
        len = send(c->fd, c->buffer_ptr, c->buffer_end - c->buffer_ptr, 0);
        if (len < 0) {
            if (ff_neterrno() != AVERROR(EAGAIN) &&
                ff_neterrno() != AVERROR(EINTR)) {
                goto close_connection;
            }
            break;
        }
        c->buffer_ptr += len;
        c->data_count += len;
        if (c->buffer_ptr >= c->buffer_end) {
            
            av_freep(&c->pb_buffer);
            start_wait_request(c, 1);
        }
        break;
    case RTSPSTATE_SEND_PACKET:
        if (c->poll_entry->revents & (POLLERR | POLLHUP)) {
            av_freep(&c->packet_buffer);
            return -1;
        }
        
        if (!(c->poll_entry->revents & POLLOUT))
            return 0;
        len = send(c->fd, c->packet_buffer_ptr,
                    c->packet_buffer_end - c->packet_buffer_ptr, 0);
        if (len < 0) {
            if (ff_neterrno() != AVERROR(EAGAIN) &&
                ff_neterrno() != AVERROR(EINTR)) {
                
                av_freep(&c->packet_buffer);
                return -1;
            }
            break;
        }
        c->packet_buffer_ptr += len;
        if (c->packet_buffer_ptr >= c->packet_buffer_end) {
            
            av_freep(&c->packet_buffer);
            c->state = RTSPSTATE_WAIT_REQUEST;
        }
        break;
    case HTTPSTATE_READY:
        
        break;
    default:
        return -1;
    }
    return 0;
close_connection:
    av_freep(&c->pb_buffer);
    return -1;
}
