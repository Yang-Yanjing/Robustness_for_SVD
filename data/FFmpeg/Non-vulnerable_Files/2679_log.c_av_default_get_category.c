AVClassCategory av_default_get_category(void *ptr)
{
    return (*(AVClass **) ptr)->category;
}
