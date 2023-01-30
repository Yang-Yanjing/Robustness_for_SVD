};
static int NewFrame( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    p_sys->p_current_picture = block_Alloc( p_sys->i_block_size );
    if( unlikely( !p_sys->p_current_picture ) )
        return VLC_ENOMEM;
    p_sys->p_y = p_sys->p_current_picture->p_buffer;
    p_sys->p_u = p_sys->p_y + p_sys->i_width * p_sys->i_height;
    p_sys->p_v = p_sys->p_u + p_sys->i_width * p_sys->i_height / 4;
    for ( int i = 0; i < MAX_AUDIOS; i++ )
    {
        sdi_audio_t *p_audio = &p_sys->p_audios[i];
        p_audio->i_left_samples = p_audio->i_right_samples = 0;
    }
    return VLC_SUCCESS;
}
