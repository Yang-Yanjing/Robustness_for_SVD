static void yae_clear(ATempoContext *atempo)
{
    atempo->size = 0;
    atempo->head = 0;
    atempo->tail = 0;
    atempo->nfrag = 0;
    atempo->state = YAE_LOAD_FRAGMENT;
    atempo->position[0] = 0;
    atempo->position[1] = 0;
    atempo->origin[0] = 0;
    atempo->origin[1] = 0;
    atempo->frag[0].position[0] = 0;
    atempo->frag[0].position[1] = 0;
    atempo->frag[0].nsamples    = 0;
    atempo->frag[1].position[0] = 0;
    atempo->frag[1].position[1] = 0;
    atempo->frag[1].nsamples    = 0;
    
    
    
    atempo->frag[0].position[0] = -(int64_t)(atempo->window / 2);
    atempo->frag[0].position[1] = -(int64_t)(atempo->window / 2);
    av_frame_free(&atempo->dst_buffer);
    atempo->dst     = NULL;
    atempo->dst_end = NULL;
    atempo->nsamples_in       = 0;
    atempo->nsamples_out      = 0;
}
