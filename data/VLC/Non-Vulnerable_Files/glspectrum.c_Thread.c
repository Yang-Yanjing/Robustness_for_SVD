 */
static void *Thread( void *p_data )
{
    filter_t  *p_filter = (filter_t*)p_data;
    filter_sys_t *p_sys = p_filter->p_sys;
    video_format_t fmt;
    vlc_gl_t *gl;
    unsigned int i_last_width = 0;
    unsigned int i_last_height = 0;
    /* Create the openGL provider */
    p_sys->p_vout =
        (vout_thread_t *)vlc_object_create(p_filter, sizeof(vout_thread_t));
    if (!p_sys->p_vout)
        goto error;
    /* Configure the video format for the opengl provider. */
    video_format_Init(&fmt, 0);
    video_format_Setup(&fmt, VLC_CODEC_RGB32, p_sys->i_width, p_sys->i_height,
                       p_sys->i_width, p_sys->i_height, 0, 1 );
    fmt.i_sar_num = 1;
    fmt.i_sar_den = 1;
    /* Init vout state. */
    vout_display_state_t state;
    memset(&state, 0, sizeof(state));
    state.cfg.display.sar.num = 1;
    state.cfg.display.sar.den = 1;
    state.cfg.is_display_filled = true;
    state.cfg.zoom.num = 1;
    state.cfg.zoom.den = 1;
    state.sar.num = 1;
    state.sar.den = 1;
    p_sys->p_vd = vout_NewDisplay(p_sys->p_vout, &fmt, &state,
                                  "opengl", 1000000, 1000000);
    if (!p_sys->p_vd)
    {
        vlc_object_release(p_sys->p_vout);
        goto error;
    }
    gl = vout_GetDisplayOpengl(p_sys->p_vd);
    if (!gl)
    {
        vout_DeleteDisplay(p_sys->p_vd, NULL);
        vlc_object_release(p_sys->p_vout);
        goto error;
    }
    vlc_sem_post(&p_sys->ready);
    vlc_gl_MakeCurrent(gl);
    initOpenGLScene();
    vlc_gl_ReleaseCurrent(gl);
    float height[NB_BANDS] = {0};
    while (1)
    {
        block_t *block = block_FifoGet(p_sys->fifo);
        int canc = vlc_savecancel();
        vlc_gl_MakeCurrent(gl);
        /* Manage the events */
        vout_ManageDisplay(p_sys->p_vd, true);
        if (p_sys->p_vd->cfg->display.width != i_last_width ||
            p_sys->p_vd->cfg->display.height != i_last_height)
        {
            /* FIXME it is not perfect as we will have black bands */
            vout_display_place_t place;
            vout_display_PlacePicture(&place, &p_sys->p_vd->source,
                                      p_sys->p_vd->cfg, false);
            i_last_width  = p_sys->p_vd->cfg->display.width;
            i_last_height = p_sys->p_vd->cfg->display.height;
        }
        /* Horizontal scale for 20-band equalizer */
        const unsigned xscale[] = {0,1,2,3,4,5,6,7,8,11,15,20,27,
                                   36,47,62,82,107,141,184,255};
        fft_state *p_state = NULL; /* internal FFT data */
        DEFINE_WIND_CONTEXT(wind_ctx); /* internal window data */
        unsigned i, j;
        float p_output[FFT_BUFFER_SIZE];           /* Raw FFT Result  */
        int16_t p_buffer1[FFT_BUFFER_SIZE];        /* Buffer on which we perform
                                                      the FFT (first channel) */
        int16_t p_dest[FFT_BUFFER_SIZE];           /* Adapted FFT result */
        float *p_buffl = (float*)block->p_buffer;  /* Original buffer */
        int16_t  *p_buffs;                         /* int16_t converted buffer */
        int16_t  *p_s16_buff;                      /* int16_t converted buffer */
        if (!block->i_nb_samples) {
            msg_Err(p_filter, "no samples yet");
            goto release;
        }
        /* Allocate the buffer only if the number of samples change */
        if (block->i_nb_samples != p_sys->i_prev_nb_samples)
        {
            free(p_sys->p_prev_s16_buff);
            p_sys->p_prev_s16_buff = malloc(block->i_nb_samples *
                                            p_sys->i_channels *
                                            sizeof(int16_t));
            if (!p_sys->p_prev_s16_buff)
                goto release;
            p_sys->i_prev_nb_samples = block->i_nb_samples;
        }
        p_buffs = p_s16_buff = p_sys->p_prev_s16_buff;
        /* Convert the buffer to int16_t
           Pasted from float32tos16.c */
        for (i = block->i_nb_samples * p_sys->i_channels; i--;)
        {
            union {float f; int32_t i;} u;
            u.f = *p_buffl + 384.0;
            if (u.i > 0x43c07fff)
                *p_buffs = 32767;
            else if (u.i < 0x43bf8000)
                *p_buffs = -32768;
            else
                *p_buffs = u.i - 0x43c00000;
            p_buffl++; p_buffs++;
        }
        p_state = visual_fft_init();
        if (!p_state)
        {
            msg_Err(p_filter,"unable to initialize FFT transform");
            goto release;
        }
        if (!window_init(FFT_BUFFER_SIZE, &p_sys->wind_param, &wind_ctx))
        {
            msg_Err(p_filter,"unable to initialize FFT window");
            goto release;
        }
        p_buffs = p_s16_buff;
        for (i = 0 ; i < FFT_BUFFER_SIZE; i++)
        {
            p_output[i] = 0;
            p_buffer1[i] = *p_buffs;
            p_buffs += p_sys->i_channels;
            if (p_buffs >= &p_s16_buff[block->i_nb_samples * p_sys->i_channels])
                p_buffs = p_s16_buff;
        }
        window_scale_in_place (p_buffer1, &wind_ctx);
        fft_perform (p_buffer1, p_output, p_state);
        for (i = 0; i< FFT_BUFFER_SIZE; ++i)
            p_dest[i] = p_output[i] *  (2 ^ 16)
                        / ((FFT_BUFFER_SIZE / 2 * 32768) ^ 2);
        for (i = 0 ; i < NB_BANDS; i++)
        {
            /* Decrease the previous size of the bar. */
            height[i] -= BAR_DECREMENT;
            if (height[i] < 0)
                height[i] = 0;
            int y = 0;
            /* We search the maximum on one scale
               to determine the current size of the bar. */
            for (j = xscale[i]; j < xscale[i + 1]; j++)
            {
                if (p_dest[j] > y)
                     y = p_dest[j];
            }
            /* Calculate the height of the bar */
            float new_height = y != 0 ? log(y) * 0.4 : 0;
            height[i] = new_height > height[i]
                        ? new_height : height[i];
        }
        /* Determine the camera rotation angle. */
        p_sys->f_rotationAngle += p_sys->f_rotationIncrement;
        if (p_sys->f_rotationAngle <= -ROTATION_MAX)
            p_sys->f_rotationIncrement = ROTATION_INCREMENT;
        else if (p_sys->f_rotationAngle >= ROTATION_MAX)
            p_sys->f_rotationIncrement = -ROTATION_INCREMENT;
        /* Render the frame. */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();
            glRotatef(p_sys->f_rotationAngle, 0, 1, 0);
            drawBars(height);
        glPopMatrix();
        /* Wait to swapp the frame on time. */
        mwait(block->i_pts + (block->i_length / 2));
        if (!vlc_gl_Lock(gl))
        {
            vlc_gl_Swap(gl);
            vlc_gl_Unlock(gl);
        }
release:
        window_close(&wind_ctx);
        fft_close(p_state);
        vlc_gl_ReleaseCurrent(gl);
        block_Release(block);
        vlc_restorecancel(canc);
    }
    assert(0);
error:
    p_sys->b_error = true;
    vlc_sem_post(&p_sys->ready);
    return NULL;
}
