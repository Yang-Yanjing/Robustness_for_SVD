static void yae_release_buffers(ATempoContext *atempo)
{
    yae_clear(atempo);
    av_freep(&atempo->frag[0].data);
    av_freep(&atempo->frag[1].data);
    av_freep(&atempo->frag[0].xdat);
    av_freep(&atempo->frag[1].xdat);
    av_freep(&atempo->buffer);
    av_freep(&atempo->hann);
    av_freep(&atempo->correlation);
    av_rdft_end(atempo->real_to_complex);
    atempo->real_to_complex = NULL;
    av_rdft_end(atempo->complex_to_real);
    atempo->complex_to_real = NULL;
}
