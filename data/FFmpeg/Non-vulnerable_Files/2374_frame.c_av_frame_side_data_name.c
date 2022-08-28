const char *av_frame_side_data_name(enum AVFrameSideDataType type)
{
    switch(type) {
    case AV_FRAME_DATA_PANSCAN:         return "AVPanScan";
    case AV_FRAME_DATA_A53_CC:          return "ATSC A53 Part 4 Closed Captions";
    case AV_FRAME_DATA_STEREO3D:        return "Stereoscopic 3d metadata";
    case AV_FRAME_DATA_MATRIXENCODING:  return "AVMatrixEncoding";
    case AV_FRAME_DATA_DOWNMIX_INFO:    return "Metadata relevant to a downmix procedure";
    case AV_FRAME_DATA_REPLAYGAIN:      return "AVReplayGain";
    case AV_FRAME_DATA_DISPLAYMATRIX:   return "3x3 displaymatrix";
    case AV_FRAME_DATA_MOTION_VECTORS:  return "Motion vectors";
    }
    return NULL;
}
