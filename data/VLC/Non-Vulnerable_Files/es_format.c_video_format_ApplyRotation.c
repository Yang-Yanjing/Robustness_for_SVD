}
void video_format_ApplyRotation( video_format_t *restrict out,
                                 const video_format_t *restrict in )
{
    *out = *in;
    video_format_TransformTo(out, ORIENT_NORMAL);
}
