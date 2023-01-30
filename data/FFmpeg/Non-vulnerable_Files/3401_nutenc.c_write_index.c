static int write_index(NUTContext *nut, AVIOContext *bc) {
    int i;
    Syncpoint dummy= { .pos= 0 };
    Syncpoint *next_node[2] = { NULL };
    int64_t startpos = avio_tell(bc);
    int64_t payload_size;
    put_tt(nut, nut->max_pts_tb, bc, nut->max_pts);
    ff_put_v(bc, nut->sp_count);
    for (i=0; i<nut->sp_count; i++) {
        av_tree_find(nut->syncpoints, &dummy, (void *) ff_nut_sp_pos_cmp, (void**)next_node);
        ff_put_v(bc, (next_node[1]->pos >> 4) - (dummy.pos>>4));
        dummy.pos = next_node[1]->pos;
    }
    for (i=0; i<nut->avf->nb_streams; i++) {
        StreamContext *nus= &nut->stream[i];
        int64_t last_pts= -1;
        int j, k;
        for (j=0; j<nut->sp_count; j++) {
            int flag;
            int n = 0;
            if (j && nus->keyframe_pts[j] == nus->keyframe_pts[j-1]) {
                av_log(nut->avf, AV_LOG_WARNING, "Multiple keyframes with same PTS\n");
                nus->keyframe_pts[j] = AV_NOPTS_VALUE;
            }
            flag = (nus->keyframe_pts[j] != AV_NOPTS_VALUE) ^ (j+1 == nut->sp_count);
            for (; j<nut->sp_count && (nus->keyframe_pts[j] != AV_NOPTS_VALUE) == flag; j++)
                n++;
            ff_put_v(bc, 1 + 2*flag + 4*n);
            for (k= j - n; k<=j && k<nut->sp_count; k++) {
                if (nus->keyframe_pts[k] == AV_NOPTS_VALUE)
                    continue;
                av_assert0(nus->keyframe_pts[k] > last_pts);
                ff_put_v(bc, nus->keyframe_pts[k] - last_pts);
                last_pts = nus->keyframe_pts[k];
            }
        }
    }
    payload_size = avio_tell(bc) - startpos + 8 + 4;
    avio_wb64(bc, 8 + payload_size + av_log2(payload_size) / 7 + 1 + 4*(payload_size > 4096));
    return 0;
}
