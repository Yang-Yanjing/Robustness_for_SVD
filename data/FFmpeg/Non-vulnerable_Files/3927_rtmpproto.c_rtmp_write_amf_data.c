static int rtmp_write_amf_data(URLContext *s, char *param, uint8_t **p)
{
    char *field, *value;
    char type;
    






    if (param[0] && param[1] == ':') {
        type = param[0];
        value = param + 2;
    } else if (param[0] == 'N' && param[1] && param[2] == ':') {
        type = param[1];
        field = param + 3;
        value = strchr(field, ':');
        if (!value)
            goto fail;
        *value = '\0';
        value++;
        ff_amf_write_field_name(p, field);
    } else {
        goto fail;
    }
    switch (type) {
    case 'B':
        ff_amf_write_bool(p, value[0] != '0');
        break;
    case 'S':
        ff_amf_write_string(p, value);
        break;
    case 'N':
        ff_amf_write_number(p, strtod(value, NULL));
        break;
    case 'Z':
        ff_amf_write_null(p);
        break;
    case 'O':
        if (value[0] != '0')
            ff_amf_write_object_start(p);
        else
            ff_amf_write_object_end(p);
        break;
    default:
        goto fail;
        break;
    }
    return 0;
fail:
    av_log(s, AV_LOG_ERROR, "Invalid AMF parameter: %s\n", param);
    return AVERROR(EINVAL);
}
