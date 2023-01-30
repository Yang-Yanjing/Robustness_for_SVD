}
static bool process_server_message ( filter_t *p_filter,
                                     rfbServerToClientMsg *msg )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    switch (msg->type)
    {
    case rfbFramebufferUpdate:
        {
            msg->fu.nRects = htons(msg->fu.nRects);
            rfbFramebufferUpdateRectHeader hdr;
            for (int i_rect = 0; i_rect < msg->fu.nRects; i_rect++)
            {
                if (!read_exact(p_filter, p_sys->i_socket, (char*)&hdr,
                    sz_rfbFramebufferUpdateRectHeader ) )
                {
                    msg_Err( p_filter, "Could not read FrameBufferUpdate header" );
                    return false;
                }
                hdr.r.x = htons(hdr.r.x);
                hdr.r.y = htons(hdr.r.y);
                hdr.r.w = htons(hdr.r.w);
                hdr.r.h = htons(hdr.r.h);
                hdr.encoding = htonl(hdr.encoding);
                switch (hdr.encoding)
                {
                case rfbEncodingRaw:
                    {
                        int i_line;
                        for (i_line = 0; i_line < hdr.r.h; i_line++)
                        {
                            if ( !read_exact( p_filter, p_sys->i_socket,
                                    p_sys->read_buffer, hdr.r.w ) )
                            {
                                msg_Err( p_filter,
                                 "Could not read FrameBufferUpdate line data" );
                               return false;
                            }
                            vlc_mutex_lock( &p_sys->lock );
                            if ( !raw_line( p_sys, hdr.r.x,
                                            hdr.r.y + i_line,
                                            hdr.r.w ) )
                            {
                                msg_Err( p_filter, "raw_line failed." );
                                vlc_mutex_unlock( &p_sys->lock );
                                return false;
                            }
                            vlc_mutex_unlock( &p_sys->lock );
                        }
                    }
                    break;
                case rfbEncodingCopyRect:
                    {
                        rfbCopyRect rect;
                        if ( !read_exact( p_filter, p_sys->i_socket,
                                          (char*)&rect,
                                          sz_rfbCopyRect ) )
                        {
                            msg_Err( p_filter, "Could not read rfbCopyRect" );
                            return false;
                        }
                        rect.srcX = htons( rect.srcX );
                        rect.srcY = htons( rect.srcY );
                        vlc_mutex_lock( &p_sys->lock );
                        if ( !copy_rect( p_sys,
                                         hdr.r.x,   hdr.r.y,
                                         hdr.r.w,   hdr.r.h,
                                         rect.srcX, rect.srcY ) )
                        {
                            msg_Err( p_filter, "copy_rect failed." );
                            vlc_mutex_unlock( &p_sys->lock );
                            return false;
                        }
                        vlc_mutex_unlock( &p_sys->lock );
                    }
                    break;
                case rfbEncodingRRE:
                    {
                        rfbRREHeader rrehdr;
                        if ( !read_exact( p_filter, p_sys->i_socket,
                                          (char*)&rrehdr,
                                          sz_rfbRREHeader ) )
                        {
                            msg_Err( p_filter, "Could not read rfbRREHeader" );
                            return false;
                        }
                        uint8_t i_pixcolor;
                        if ( !read_exact(p_filter, p_sys->i_socket,
                                         (char*)&i_pixcolor, 1 ) )
                        {
                            msg_Err( p_filter, "Could not read RRE pixcolor" );
                            return false;
                        }
                        vlc_mutex_lock( &p_sys->lock );
                        if ( !fill_rect( p_sys,
                                        hdr.r.x, hdr.r.y,
                                        hdr.r.w, hdr.r.h,
                                        i_pixcolor) )
                        {
                            msg_Err( p_filter, "main fill_rect failed." );
                            vlc_mutex_unlock( &p_sys->lock );
                            return false;
                        }
                        vlc_mutex_unlock( &p_sys->lock );
                        rrehdr.nSubrects = htonl(rrehdr.nSubrects);
                        int i_datasize = rrehdr.nSubrects *
                                     ( sizeof(i_pixcolor) + sz_rfbRectangle ) ;
                        if ( i_datasize > READ_BUFFER_SIZE )
                        {
                            msg_Err( p_filter, "Buffer too small, "
                                     "need %u bytes", i_datasize );
                            return false;
                        }
                        if ( !read_exact( p_filter, p_sys->i_socket,
                                       p_sys->read_buffer, i_datasize ) )
                        {
                            msg_Err( p_filter,
                                     "Could not read RRE subrect data" );
                            return false;
                        }
                        uint32_t i_subrect;
                        rfbRectangle* p_subrect;
                        int i_offset = 0;
                        vlc_mutex_lock( &p_sys->lock );
                        for ( i_subrect = 0;
                              i_subrect < rrehdr.nSubrects; i_subrect++)
                        {
                            i_pixcolor = p_sys->read_buffer[i_offset];
                            i_offset += sizeof(i_pixcolor);
                            p_subrect =
                               (rfbRectangle*)(p_sys->read_buffer + i_offset);
                            i_offset += sz_rfbRectangle;
                            if (!fill_rect( p_sys,
                                            htons(p_subrect->x) + hdr.r.x,
                                            htons(p_subrect->y) + hdr.r.y,
                                            htons(p_subrect->w),
                                            htons(p_subrect->h),
                                            i_pixcolor) )
                            {
                                msg_Err( p_filter,
                                    "subrect %u fill_rect failed.", i_subrect );
                                vlc_mutex_unlock( &p_sys->lock );
                                return false;
                            }
                        }
                        vlc_mutex_unlock( &p_sys->lock );
                    }
                    break;
                }
            }
            vlc_mutex_lock( &p_sys->lock );
            p_sys->b_need_update = true;
            vlc_mutex_unlock( &p_sys->lock );
        }
        return true;
    case rfbSetColourMapEntries:
        {
            msg->scme.nColours = htons(msg->scme.nColours);
            msg->scme.firstColour = htons(msg->scme.firstColour);
            int i_datasize;
            if ( p_sys->b_alpha_from_vnc )
            {
                i_datasize = 2 * msg->scme.nColours * 4;
            }
            else
            {
                i_datasize = 2 * msg->scme.nColours * 3;
            }
            if ( i_datasize > READ_BUFFER_SIZE )
            {
                msg_Err( p_filter, "Buffer too small, need %u bytes",
                                   i_datasize );
                return false;
            }
            if ( !read_exact( p_filter, p_sys->i_socket,
                              p_sys->read_buffer, i_datasize ) )
            {
                msg_Err( p_filter, "Could not read color map data" );
                return false;
            }
            uint8_t i_red, i_green, i_blue, i_alpha, i_color_index;
            uint16_t i_offset = 0;
            i_alpha = 255;
            for (int i = 0; i < msg->scme.nColours; i++)
            {
                i_color_index = i+msg->scme.firstColour;
                if ( p_sys->b_alpha_from_vnc )
                {
                    i_alpha = p_sys->read_buffer[i_offset];
                    i_offset += 2;
                }
                i_red   = p_sys->read_buffer[i_offset];
                i_offset += 2;
                i_green = p_sys->read_buffer[i_offset];
                i_offset += 2;
                i_blue  = p_sys->read_buffer[i_offset];
                i_offset += 2;
                rgb_to_yuv( &p_sys->ar_color_table_yuv[i_color_index][0],
                            &p_sys->ar_color_table_yuv[i_color_index][1],
                            &p_sys->ar_color_table_yuv[i_color_index][2],
                            i_red,
                            i_green,
                            i_blue );
                p_sys->ar_color_table_yuv[i][3] = i_alpha;
            }
        }
        return true;
    case rfbBell:
        msg_Err( p_filter, "rfbBell received" );
        return true;
    case rfbServerCutText:
        msg->sct.length = htons(msg->sct.length);
        if ( msg->sct.length > READ_BUFFER_SIZE )
        {
            msg_Err( p_filter, "Buffer too small, need %u bytes", msg->sct.length );
            return false;
        }
        if ( !read_exact(p_filter, p_sys->i_socket,
                         p_sys->read_buffer, msg->sct.length ) )
        {
            msg_Err( p_filter, "Could not read Reading rfbServerCutText data" );
            return false;
        }
        return true;
    case rfbReSizeFrameBuffer:
        msg_Err( p_filter, "Reading rfbReSizeFrameBuffer not implemented" );
        return false;
    default:
        msg_Err( p_filter, "Invalid message %u received", msg->type );
        return false;
    }
    return false;
}
