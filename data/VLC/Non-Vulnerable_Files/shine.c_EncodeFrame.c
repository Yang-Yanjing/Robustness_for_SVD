}
static block_t *EncodeFrame( encoder_t *p_enc, block_t *p_block )
{
    if (!p_block) /* TODO: flush */
        return NULL;
    encoder_sys_t *p_sys = p_enc->p_sys;
    block_t *p_pcm_block;
    block_t *p_chain = NULL;
    unsigned int i_samples = p_block->i_buffer >> 2 /* s16l stereo */;
    mtime_t start_date = p_block->i_pts;
    start_date -= (mtime_t)i_samples * (mtime_t)1000000 / (mtime_t)p_enc->fmt_out.audio.i_rate;
    VLC_UNUSED(p_enc);
    do {
        p_pcm_block = GetPCM( p_enc, p_block );
        if( !p_pcm_block )
            break;
        p_block = NULL; /* we don't need it anymore */
        int16_t pcm_planar_buf[SHINE_MAX_SAMPLES * 2];
        int16_t *pcm_planar_buf_chans[2] = {
            &pcm_planar_buf[0],
            &pcm_planar_buf[p_sys->samples_per_frame],
        };
        aout_Deinterleave( pcm_planar_buf, p_pcm_block->p_buffer,
                p_sys->samples_per_frame, p_enc->fmt_in.audio.i_channels, p_enc->fmt_in.i_codec);
        long written;
        unsigned char *buf = shine_encode_buffer(p_sys->s, pcm_planar_buf_chans, &written);
        block_Release( p_pcm_block );
        if (written <= 0)
            break;
        block_t *p_mp3_block = block_Alloc( written );
        if( !p_mp3_block )
            break;
        memcpy( p_mp3_block->p_buffer, buf, written );
        /* date management */
        p_mp3_block->i_length = p_sys->samples_per_frame * 1000000 /
            p_enc->fmt_out.audio.i_rate;
        start_date += p_mp3_block->i_length;
        p_mp3_block->i_dts = p_mp3_block->i_pts = start_date;
        p_mp3_block->i_nb_samples = p_sys->samples_per_frame;
        block_ChainAppend( &p_chain, p_mp3_block );
    } while( p_pcm_block );
    return p_chain;
}
