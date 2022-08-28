AVStereo3D *av_stereo3d_create_side_data(AVFrame *frame)
{
    AVFrameSideData *side_data = av_frame_new_side_data(frame,
                                                        AV_FRAME_DATA_STEREO3D,
                                                        sizeof(AVStereo3D));
    if (!side_data)
        return NULL;
    memset(side_data->data, 0, sizeof(AVStereo3D));
    return (AVStereo3D *)side_data->data;
}
