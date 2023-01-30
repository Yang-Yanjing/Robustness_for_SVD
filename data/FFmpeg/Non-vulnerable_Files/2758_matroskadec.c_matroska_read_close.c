static int matroska_read_close(AVFormatContext *s)
{
    MatroskaDemuxContext *matroska = s->priv_data;
    MatroskaTrack *tracks = matroska->tracks.elem;
    int n;
    matroska_clear_queue(matroska);
    for (n = 0; n < matroska->tracks.nb_elem; n++)
        if (tracks[n].type == MATROSKA_TRACK_TYPE_AUDIO)
            av_freep(&tracks[n].audio.buf);
    ebml_free(matroska_cluster, &matroska->current_cluster);
    ebml_free(matroska_segment, matroska);
    return 0;
}
