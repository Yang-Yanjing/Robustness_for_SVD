//Simplify transforms to have something more managable. Order: angle, hflip.
static void transform_GetBasicOps( video_transform_t transform,
                                   unsigned *restrict angle,
                                   bool *restrict hflip )
{
    *hflip = ORIENT_IS_MIRROR(transform);
    switch ( transform )
    {
        case TRANSFORM_R90:
        case TRANSFORM_TRANSPOSE:
            *angle = 90;
            break;
        case TRANSFORM_R180:
        case TRANSFORM_VFLIP:
            *angle = 180;
            break;
        case TRANSFORM_R270:
        case TRANSFORM_ANTI_TRANSPOSE:
            *angle = 270;
            break;
        case TRANSFORM_HFLIP:
        case TRANSFORM_IDENTITY:
            *angle = 0;
            break;
    }
}
