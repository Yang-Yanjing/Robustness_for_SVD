static const char *rep2str(RepeatedField repeated_field)
{
    switch(repeated_field) {
        case REPEAT_NONE    : return "neither";
        case REPEAT_TOP     : return "top";
        case REPEAT_BOTTOM  : return "bottom";
    }
    return NULL;
}
