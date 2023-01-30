}
video_transform_t video_format_GetTransform( video_orientation_t src,
                                             video_orientation_t dst )
{
    unsigned angle1, angle2;
    bool hflip1, hflip2;
    transform_GetBasicOps(  (video_transform_t)src, &angle1, &hflip1 );
    transform_GetBasicOps( transform_Inverse( (video_transform_t)dst ),
                           &angle2, &hflip2 );
    int angle = (angle1 + angle2) % 360;
    bool hflip = hflip1 ^ hflip2;
    return transform_FromBasicOps(angle, hflip);
}
