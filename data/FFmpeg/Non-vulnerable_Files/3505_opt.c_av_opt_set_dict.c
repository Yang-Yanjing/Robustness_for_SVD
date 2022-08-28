int av_opt_set_dict(void *obj, AVDictionary **options)
{
    return av_opt_set_dict2(obj, options, 0);
}
