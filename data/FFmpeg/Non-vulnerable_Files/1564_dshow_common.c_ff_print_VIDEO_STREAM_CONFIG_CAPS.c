void ff_print_VIDEO_STREAM_CONFIG_CAPS(const VIDEO_STREAM_CONFIG_CAPS *caps)
{
#if DSHOWDEBUG
    dshowdebug(" VIDEO_STREAM_CONFIG_CAPS\n");
    dshowdebug("  guid\t");
    ff_printGUID(&caps->guid);
    dshowdebug("\n");
    dshowdebug("  VideoStandard\t%lu\n", caps->VideoStandard);
    dshowdebug("  InputSize %ld\t%ld\n", caps->InputSize.cx, caps->InputSize.cy);
    dshowdebug("  MinCroppingSize %ld\t%ld\n", caps->MinCroppingSize.cx, caps->MinCroppingSize.cy);
    dshowdebug("  MaxCroppingSize %ld\t%ld\n", caps->MaxCroppingSize.cx, caps->MaxCroppingSize.cy);
    dshowdebug("  CropGranularityX\t%d\n", caps->CropGranularityX);
    dshowdebug("  CropGranularityY\t%d\n", caps->CropGranularityY);
    dshowdebug("  CropAlignX\t%d\n", caps->CropAlignX);
    dshowdebug("  CropAlignY\t%d\n", caps->CropAlignY);
    dshowdebug("  MinOutputSize %ld\t%ld\n", caps->MinOutputSize.cx, caps->MinOutputSize.cy);
    dshowdebug("  MaxOutputSize %ld\t%ld\n", caps->MaxOutputSize.cx, caps->MaxOutputSize.cy);
    dshowdebug("  OutputGranularityX\t%d\n", caps->OutputGranularityX);
    dshowdebug("  OutputGranularityY\t%d\n", caps->OutputGranularityY);
    dshowdebug("  StretchTapsX\t%d\n", caps->StretchTapsX);
    dshowdebug("  StretchTapsY\t%d\n", caps->StretchTapsY);
    dshowdebug("  ShrinkTapsX\t%d\n", caps->ShrinkTapsX);
    dshowdebug("  ShrinkTapsY\t%d\n", caps->ShrinkTapsY);
    dshowdebug("  MinFrameInterval\t%"PRId64"\n", caps->MinFrameInterval);
    dshowdebug("  MaxFrameInterval\t%"PRId64"\n", caps->MaxFrameInterval);
    dshowdebug("  MinBitsPerSecond\t%ld\n", caps->MinBitsPerSecond);
    dshowdebug("  MaxBitsPerSecond\t%ld\n", caps->MaxBitsPerSecond);
#endif
}
