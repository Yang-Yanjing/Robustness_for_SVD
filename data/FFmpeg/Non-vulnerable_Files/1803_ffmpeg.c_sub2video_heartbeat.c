static void sub2video_heartbeat(InputStream *ist, int64_t pts)
{
    InputFile *infile = input_files[ist->file_index];
    int i, j, nb_reqs;
    int64_t pts2;
    



    for (i = 0; i < infile->nb_streams; i++) {
        InputStream *ist2 = input_streams[infile->ist_index + i];
        if (!ist2->sub2video.frame)
            continue;
        

        pts2 = av_rescale_q(pts, ist->st->time_base, ist2->st->time_base) - 1;
        
        if (pts2 <= ist2->sub2video.last_pts)
            continue;
        if (pts2 >= ist2->sub2video.end_pts || !ist2->sub2video.frame->data[0])
            sub2video_update(ist2, NULL);
        for (j = 0, nb_reqs = 0; j < ist2->nb_filters; j++)
            nb_reqs += av_buffersrc_get_nb_failed_requests(ist2->filters[j]->filter);
        if (nb_reqs)
            sub2video_push_ref(ist2, pts2);
    }
}
