}
static int process_options( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    char *psz_dup;
    char *psz_parser;
    char *token = NULL;
    char *state = NULL;
    const char *in_size = NULL;
    const char *in_fmt = NULL;
    float rate_f;
    psz_dup = strdup( p_demux->psz_location );
    psz_parser = psz_dup;
    for( token = strtok_r( psz_parser,":",&state); token;
         token = strtok_r( NULL, ":", &state ) )
    {
        if( strncmp( token, "size=", strlen("size=") ) == 0 )
        {
            token += strlen("size=");
            if( strncmp( token, "160x120", 7 ) == 0 )
            {
                /* TODO - UYV444 chroma converter is needed ...
                    * video size of 160x120 is temporarily disabled
                    */
                msg_Err( p_demux,
                    "video size of 160x120 is actually disabled for lack of"
                    "chroma support. It will relased ASAP, until then try "
                    "an higher size (320x240 and 640x480 are fully supported)" );
                free(psz_dup);
                return VLC_EGENERIC;
#if 0
                in_size = "160x120";
                p_sys->width = 160;
                p_sys->height = 120;
#endif
            }
            else if( strncmp( token, "320x240", 7 ) == 0 )
            {
                in_size = "320x240";
                p_sys->width = 320;
                p_sys->height = 240;
            }
            else if( strncmp( token, "640x480", 7 ) == 0 )
            {
                in_size = "640x480";
                p_sys->width = 640;
                p_sys->height = 480;
            }
            else
            {
                msg_Err( p_demux,
                    "This program currently suppots frame sizes of"
                    " 160x120, 320x240, and 640x480. "
                    "Please specify one of them. You have specified %s.",
                    token );
                free(psz_dup);
                return VLC_EGENERIC;
            }
            msg_Dbg( p_demux, "Requested video size : %s",token );
        }
        if( strncmp( token, "format=", strlen("format=") ) == 0 )
        {
            token += strlen("format=");
            if( strncmp( token, "YUV411", 6 ) == 0 )
            {
                in_fmt = "YUV411";
            }
            else if( strncmp( token, "YUV422", 6 ) == 0 )
            {
                in_fmt = "YUV422";
            }
            else if( strncmp( token, "YUV444", 6 ) == 0 )
            {
                in_fmt = "YUV444";
            }
            else if( strncmp( token, "RGB8", 4 ) == 0 )
            {
                in_fmt = "RGB8";
            }
            else if( strncmp( token, "MONO8", 5 ) == 0 )
            {
                in_fmt = "MONO8";
            }
            else if( strncmp( token, "MONO16", 6 ) == 0 )
            {
                in_fmt = "MONO16";
            }
            else
            {
                msg_Err( p_demux, "Invalid format %s.", token );
                free(psz_dup);
                return VLC_EGENERIC;
            }
            msg_Dbg( p_demux, "Requested video format : %s", token );
        }
        else if( strncmp( token, "fps=", strlen( "fps=" ) ) == 0 )
        {
            token += strlen("fps=");
            sscanf( token, "%g", &rate_f );
            if( rate_f == 1.875 )
                p_sys->frame_rate = DC1394_FRAMERATE_1_875;
            else if( rate_f == 3.75 )
                p_sys->frame_rate = DC1394_FRAMERATE_3_75;
            else if( rate_f == 7.5 )
                p_sys->frame_rate = DC1394_FRAMERATE_7_5;
            else if( rate_f == 15 )
                p_sys->frame_rate = DC1394_FRAMERATE_15;
            else if( rate_f == 30 )
                p_sys->frame_rate = DC1394_FRAMERATE_30;
            else if( rate_f == 60 )
                p_sys->frame_rate = DC1394_FRAMERATE_60;
            else
            {
                msg_Err( p_demux ,
                    "This program supports framerates of"
                    " 1.875, 3.75, 7.5, 15, 30, 60. "
                    "Please specify one of them. You have specified %s.",
                    token);
                free(psz_dup);
                return VLC_EGENERIC;
            }
            msg_Dbg( p_demux, "Requested frame rate : %s",token );
        }
        else if( strncmp( token, "resetbus", strlen( "resetbus" ) ) == 0 )
        {
            token += strlen("resetbus");
            p_sys->reset_bus = 1;
        }
        else if( strncmp( token, "brightness=", strlen( "brightness=" ) ) == 0 )
        {
            int nr = 0;
            token += strlen("brightness=");
            nr = sscanf( token, "%u", &p_sys->brightness);
            if( nr != 1 )
            {
                msg_Err( p_demux, "Bad brightness value '%s', "
                                  "must be an unsigned integer.",
                                  token );
                free(psz_dup);
                return VLC_EGENERIC;
            }
        }
        else if( strncmp( token, "buffers=", strlen( "buffers=" ) ) == 0 )
        {
            int nr = 0;
            int in_buf = 0;
            token += strlen("buffers=");
            nr = sscanf( token, "%d", &in_buf);
            if( nr != 1 || in_buf < 1 )
            {
                msg_Err( p_demux, "DMA buffers must be 1 or greater." );
                free(psz_dup);
                return VLC_EGENERIC;
            }
            else p_sys->dma_buffers = in_buf;
        }
#if 0
        // NOTE: If controller support is added back, more logic will needed to be added
        //       after the cameras are scanned.
        else if( strncmp( token, "controller=", strlen( "controller=" ) ) == 0 )
        {
            int nr = 0;
            token += strlen("controller=");
            nr = sscanf( token, "%u", &p_sys->controller );
            if( nr != 1)
            {
                msg_Err(p_demux, "Bad controller value '%s', "
                                 "must be an unsigned integer.",
                                 token );
                return VLC_EGENERIC;
            }
        }
#endif
        else if( strncmp( token, "camera=", strlen( "camera=" ) ) == 0 )
        {
            int nr = 0;
            token += strlen("camera=");
            nr = sscanf(token,"%u",&p_sys->selected_camera);
            if( nr != 1)
            {
                msg_Err( p_demux, "Bad camera number '%s', "
                                  "must be an unsigned integer.",
                                  token );
                free(psz_dup);
                return VLC_EGENERIC;
            }
        }
        else if( strncmp( token, "vdev=", strlen( "vdev=" ) ) == 0)
        {
            token += strlen("vdev=");
            p_sys->video_device = strdup(token);
            msg_Dbg( p_demux, "Using video device '%s'.", token );
        }
        else if( strncmp( token, "focus=", strlen("focus=" ) ) == 0)
        {
            int nr = 0;
            token += strlen("focus=");
            nr = sscanf( token, "%u", &p_sys->focus );
            if( nr != 1 )
            {
                msg_Err( p_demux, "Bad focus value '%s', "
                                  "must be an unsigned integer.",
                                  token );
                free(psz_dup);
                return VLC_EGENERIC;
            }
        }
        else if( strncmp( token, "uid=", strlen("uid=") ) == 0)
        {
            token += strlen("uid=");
            sscanf( token, "0x%"SCNx64, &p_sys->selected_uid );
        }
    }
    // The mode is a combination of size and format and not every format
    // is supported by every size.
    if( in_size)
    {
        if( strcmp( in_size, "160x120") == 0)
        {
            if( in_fmt && (strcmp( in_fmt, "YUV444") != 0) )
                msg_Err(p_demux, "160x120 only supports YUV444 - forcing");
            p_sys->video_mode = DC1394_VIDEO_MODE_160x120_YUV444;
        }
        else if( strcmp( in_size, "320x240") == 0)
        {
            if( in_fmt && (strcmp( in_fmt, "YUV422") != 0) )
                msg_Err(p_demux, "320x240 only supports YUV422 - forcing");
            p_sys->video_mode = DC1394_VIDEO_MODE_320x240_YUV422;
        }
    }
    else
    { // 640x480 default
        if( in_fmt )
        {
            if( strcmp( in_fmt, "RGB8") == 0)
                p_sys->video_mode = DC1394_VIDEO_MODE_640x480_RGB8;
            else if( strcmp( in_fmt, "MONO8") == 0)
                p_sys->video_mode = DC1394_VIDEO_MODE_640x480_MONO8;
            else if( strcmp( in_fmt, "MONO16") == 0)
                p_sys->video_mode = DC1394_VIDEO_MODE_640x480_MONO16;
            else if( strcmp( in_fmt, "YUV411") == 0)
                p_sys->video_mode = DC1394_VIDEO_MODE_640x480_YUV411;
            else // YUV422 default
                p_sys->video_mode = DC1394_VIDEO_MODE_640x480_YUV422;
        }
        else // YUV422 default
            p_sys->video_mode = DC1394_VIDEO_MODE_640x480_YUV422;
    }
    free( psz_dup );
    return VLC_SUCCESS;
}
