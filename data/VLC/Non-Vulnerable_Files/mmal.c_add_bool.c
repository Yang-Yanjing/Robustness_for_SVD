    add_integer(MMAL_LAYER_NAME, 1, MMAL_LAYER_TEXT, MMAL_LAYER_LONGTEXT, false)
    add_bool(MMAL_ADJUST_REFRESHRATE_NAME, false, MMAL_ADJUST_REFRESHRATE_TEXT,
                    MMAL_ADJUST_REFRESHRATE_LONGTEXT, false)
    set_callbacks(Open, Close)
vlc_module_end()
struct dmx_region_t {
    struct dmx_region_t *next;
    picture_t *picture;
    VC_RECT_T bmp_rect;
    VC_RECT_T src_rect;
    VC_RECT_T dst_rect;
    VC_DISPMANX_ALPHA_T alpha;
    DISPMANX_ELEMENT_HANDLE_T element;
    DISPMANX_RESOURCE_HANDLE_T resource;
    int32_t pos_x;
    int32_t pos_y;
};
