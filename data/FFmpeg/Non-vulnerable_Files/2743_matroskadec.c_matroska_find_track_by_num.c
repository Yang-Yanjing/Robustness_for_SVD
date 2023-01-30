static MatroskaTrack *matroska_find_track_by_num(MatroskaDemuxContext *matroska,
                                                 int num)
{
    MatroskaTrack *tracks = matroska->tracks.elem;
    int i;
    for (i = 0; i < matroska->tracks.nb_elem; i++)
        if (tracks[i].num == num)
            return &tracks[i];
    av_log(matroska->ctx, AV_LOG_ERROR, "Invalid track number %d\n", num);
    return NULL;
}
