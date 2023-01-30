static av_always_inline void spatial_frame_pack(FramepackContext *s, AVFrame *dst)
{
    switch (s->format) {
    case AV_STEREO3D_SIDEBYSIDE:
        horizontal_frame_pack(s, dst, 0);
        break;
    case AV_STEREO3D_COLUMNS:
        horizontal_frame_pack(s, dst, 1);
        break;
    case AV_STEREO3D_TOPBOTTOM:
        vertical_frame_pack(s, dst, 0);
        break;
    case AV_STEREO3D_LINES:
        vertical_frame_pack(s, dst, 1);
        break;
    }
}
