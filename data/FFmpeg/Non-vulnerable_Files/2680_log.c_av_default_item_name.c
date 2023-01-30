const char *av_default_item_name(void *ptr)
{
    return (*(AVClass **) ptr)->class_name;
}
