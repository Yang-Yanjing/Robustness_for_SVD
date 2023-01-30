}
vout_display_t *vout_NewSplitter(vout_thread_t *vout,
                                 const video_format_t *source,
                                 const vout_display_state_t *state,
                                 const char *module,
                                 const char *splitter_module,
                                 mtime_t double_click_timeout,
                                 mtime_t hide_timeout)
{
    video_splitter_t *splitter =
        video_splitter_New(VLC_OBJECT(vout), splitter_module, source);
    if (!splitter)
        return NULL;
    /* */
    vout_display_t *wrapper =
        DisplayNew(vout, source, state, module, true, NULL,
                    double_click_timeout, hide_timeout, NULL);
    if (!wrapper) {
        video_splitter_Delete(splitter);
        return NULL;
    }
    vout_display_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        abort();
    sys->picture = calloc(splitter->i_output, sizeof(*sys->picture));
    if (!sys->picture )
        abort();
    sys->splitter = splitter;
    sys->pool     = NULL;
    wrapper->pool    = SplitterPool;
    wrapper->prepare = SplitterPrepare;
    wrapper->display = SplitterDisplay;
    wrapper->control = SplitterControl;
    wrapper->manage  = SplitterManage;
    wrapper->sys     = sys;
    /* */
    video_splitter_owner_t *owner = malloc(sizeof(*owner));
    if (!owner)
        abort();
    owner->wrapper = wrapper;
    splitter->p_owner = owner;
    splitter->pf_picture_new = SplitterPictureNew;
    splitter->pf_picture_del = SplitterPictureDel;
    /* */
    TAB_INIT(sys->count, sys->display);
    for (int i = 0; i < splitter->i_output; i++) {
        vout_display_owner_t owner;
        owner.event      = SplitterEvent;
        owner.window_new = SplitterNewWindow;
        owner.window_del = SplitterDelWindow;
        const video_splitter_output_t *output = &splitter->p_output[i];
        vout_display_state_t ostate;
        memset(&ostate, 0, sizeof(ostate));
        ostate.cfg.is_fullscreen = false;
        ostate.cfg.display = state->cfg.display;
        ostate.cfg.align.horizontal = 0; /* TODO */
        ostate.cfg.align.vertical = 0; /* TODO */
        ostate.cfg.is_display_filled = true;
        ostate.cfg.zoom.num = 1;
        ostate.cfg.zoom.den = 1;
        vout_display_t *vd = DisplayNew(vout, &output->fmt, &ostate,
                                           output->psz_module ? output->psz_module : module,
                                           false, wrapper,
                                           double_click_timeout, hide_timeout, &owner);
        if (!vd) {
            vout_DeleteDisplay(wrapper, NULL);
            return NULL;
        }
        TAB_APPEND(sys->count, sys->display, vd);
    }
    return wrapper;
}
