}
void video_format_TransformTo( video_format_t *restrict fmt,
                               video_orientation_t dst_orientation )
{
    video_transform_t transform = video_format_GetTransform(fmt->orientation,
                                                            dst_orientation);
    video_format_TransformBy(fmt, transform);
}
