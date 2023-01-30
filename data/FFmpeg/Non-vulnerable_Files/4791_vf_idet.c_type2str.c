AVFILTER_DEFINE_CLASS(idet);
static const char *type2str(Type type)
{
    switch(type) {
        case TFF          : return "tff";
        case BFF          : return "bff";
        case PROGRESSIVE  : return "progressive";
        case UNDETERMINED : return "undetermined";
    }
    return NULL;
}
