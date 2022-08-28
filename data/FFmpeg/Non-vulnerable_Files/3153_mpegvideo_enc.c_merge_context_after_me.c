static void merge_context_after_me(MpegEncContext *dst, MpegEncContext *src){
    MERGE(me.scene_change_score);
    MERGE(me.mc_mb_var_sum_temp);
    MERGE(me.mb_var_sum_temp);
}
