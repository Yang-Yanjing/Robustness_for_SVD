}
static video_transform_t transform_FromBasicOps( unsigned angle, bool hflip )
{
    switch ( angle )
    {
        case 90:
            return hflip ? TRANSFORM_TRANSPOSE : TRANSFORM_R90;
        case 180:
            return hflip ? TRANSFORM_VFLIP : TRANSFORM_R180;
        case 270:
            return hflip ? TRANSFORM_ANTI_TRANSPOSE : TRANSFORM_R270;
        default:
            return hflip ? TRANSFORM_HFLIP : TRANSFORM_IDENTITY;
    }
}
