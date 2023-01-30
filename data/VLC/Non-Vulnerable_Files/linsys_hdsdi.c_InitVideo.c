}
static int InitVideo( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    es_format_t fmt;
    msg_Dbg( p_demux, "found standard %d", p_sys->i_standard );
    switch ( p_sys->i_standard )
    {
    case SDIVIDEO_CTL_BT_601_576I_50HZ:
        /* PAL */
        p_sys->i_frame_rate      = 25;
        p_sys->i_frame_rate_base = 1;
        p_sys->i_width           = 720;
        p_sys->i_height          = 576;
        p_sys->i_aspect          = 4 * VOUT_ASPECT_FACTOR / 3;
        break;
    case SDIVIDEO_CTL_SMPTE_296M_720P_50HZ:
        p_sys->i_frame_rate      = 50;
        p_sys->i_frame_rate_base = 1;
        p_sys->i_width           = 1280;
        p_sys->i_height          = 720;
        p_sys->i_aspect          = 16 * VOUT_ASPECT_FACTOR / 9;
        break;
    case SDIVIDEO_CTL_SMPTE_296M_720P_60HZ:
        p_sys->i_frame_rate      = 60;
        p_sys->i_frame_rate_base = 1;
        p_sys->i_width           = 1280;
        p_sys->i_height          = 720;
        p_sys->i_aspect          = 16 * VOUT_ASPECT_FACTOR / 9;
        break;
    case SDIVIDEO_CTL_SMPTE_295M_1080I_50HZ:
    case SDIVIDEO_CTL_SMPTE_274M_1080I_50HZ:
    case SDIVIDEO_CTL_SMPTE_274M_1080PSF_25HZ:
        /* 1080i50 or 1080p25 */
        p_sys->i_frame_rate      = 25;
        p_sys->i_frame_rate_base = 1;
        p_sys->i_width           = 1920;
        p_sys->i_height          = 1080;
        p_sys->i_aspect          = 16 * VOUT_ASPECT_FACTOR / 9;
        break;
    case SDIVIDEO_CTL_SMPTE_274M_1080I_59_94HZ:
        p_sys->i_frame_rate      = 30000;
        p_sys->i_frame_rate_base = 1001;
        p_sys->i_width           = 1920;
        p_sys->i_height          = 1080;
        p_sys->i_aspect          = 16 * VOUT_ASPECT_FACTOR / 9;
        break;
    case SDIVIDEO_CTL_SMPTE_274M_1080I_60HZ:
        p_sys->i_frame_rate      = 30;
        p_sys->i_frame_rate_base = 1;
        p_sys->i_width           = 1920;
        p_sys->i_height          = 1080;
        p_sys->i_aspect          = 16 * VOUT_ASPECT_FACTOR / 9;
        break;
    default:
        msg_Err( p_demux, "unsupported standard %d", p_sys->i_standard );
        return VLC_EGENERIC;
    }
    p_sys->i_next_vdate = START_DATE;
    p_sys->i_incr = 1000000 * p_sys->i_frame_rate_base / p_sys->i_frame_rate;
    p_sys->i_vblock_size = p_sys->i_width * p_sys->i_height * 3 / 2
                            + sizeof(struct block_extension_t);
    /* Video ES */
    es_format_Init( &fmt, VIDEO_ES, VLC_FOURCC('I','4','2','0') );
    fmt.i_id                    = p_sys->i_id_video;
    fmt.video.i_frame_rate      = p_sys->i_frame_rate;
    fmt.video.i_frame_rate_base = p_sys->i_frame_rate_base;
    fmt.video.i_width           = fmt.video.i_visible_width = p_sys->i_width;
    fmt.video.i_height          = fmt.video.i_visible_height = p_sys->i_height;
    fmt.video.i_sar_num         = p_sys->i_aspect * fmt.video.i_height
                                  / fmt.video.i_width;
    fmt.video.i_sar_den         = VOUT_ASPECT_FACTOR;
    p_sys->p_es_video           = es_out_Add( p_demux->out, &fmt );
    return VLC_SUCCESS;
}
