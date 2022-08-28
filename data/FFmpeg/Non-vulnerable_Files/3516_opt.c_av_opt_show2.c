int av_opt_show2(void *obj, void *av_log_obj, int req_flags, int rej_flags)
{
    if (!obj)
        return -1;
    av_log(av_log_obj, AV_LOG_INFO, "%s AVOptions:\n", (*(AVClass**)obj)->class_name);
    opt_list(obj, av_log_obj, NULL, req_flags, rej_flags);
    return 0;
}
