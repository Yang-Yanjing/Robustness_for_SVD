static void print_option(const AVOption *opts, const AVOption *o, int per_stream)
{
    if (!(o->flags & (AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_ENCODING_PARAM)))
        return;
    printf("@item -%s%s @var{", o->name, per_stream ? "[:stream_specifier]" : "");
    switch (o->type) {
    case AV_OPT_TYPE_BINARY:   printf("hexadecimal string"); break;
    case AV_OPT_TYPE_STRING:   printf("string");             break;
    case AV_OPT_TYPE_INT:
    case AV_OPT_TYPE_INT64:    printf("integer");            break;
    case AV_OPT_TYPE_FLOAT:
    case AV_OPT_TYPE_DOUBLE:   printf("float");              break;
    case AV_OPT_TYPE_RATIONAL: printf("rational number");    break;
    case AV_OPT_TYPE_FLAGS:    printf("flags");              break;
    default:                   printf("value");              break;
    }
    printf("} (@emph{");
    if (o->flags & AV_OPT_FLAG_DECODING_PARAM) {
        printf("input");
        if (o->flags & AV_OPT_FLAG_ENCODING_PARAM)
            printf("/");
    }
    if (o->flags & AV_OPT_FLAG_ENCODING_PARAM) printf("output");
    if (o->flags & AV_OPT_FLAG_AUDIO_PARAM)    printf(",audio");
    if (o->flags & AV_OPT_FLAG_VIDEO_PARAM)    printf(",video");
    if (o->flags & AV_OPT_FLAG_SUBTITLE_PARAM) printf(",subtitles");
    printf("})\n");
    if (o->help)
        printf("%s\n", o->help);
    if (o->unit) {
        const AVOption *u;
        printf("\nPossible values:\n@table @samp\n");
        for (u = opts; u->name; u++) {
            if (u->type == AV_OPT_TYPE_CONST && u->unit && !strcmp(u->unit, o->unit))
                printf("@item %s\n%s\n", u->name, u->help ? u->help : "");
        }
        printf("@end table\n");
    }
}
