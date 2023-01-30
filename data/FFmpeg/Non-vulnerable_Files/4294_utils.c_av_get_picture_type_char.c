char av_get_picture_type_char(enum AVPictureType pict_type)
{
    switch (pict_type) {
    case AV_PICTURE_TYPE_I:  return 'I';
    case AV_PICTURE_TYPE_P:  return 'P';
    case AV_PICTURE_TYPE_B:  return 'B';
    case AV_PICTURE_TYPE_S:  return 'S';
    case AV_PICTURE_TYPE_SI: return 'i';
    case AV_PICTURE_TYPE_SP: return 'p';
    case AV_PICTURE_TYPE_BI: return 'b';
    default:                 return '?';
    }
}
