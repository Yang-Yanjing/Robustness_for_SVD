static void hvcc_init(HEVCDecoderConfigurationRecord *hvcc)
{
    memset(hvcc, 0, sizeof(HEVCDecoderConfigurationRecord));
    hvcc->configurationVersion = 1;
    hvcc->lengthSizeMinusOne   = 3; 
    



    hvcc->general_profile_compatibility_flags = 0xffffffff;
    hvcc->general_constraint_indicator_flags  = 0xffffffffffff;
    



    hvcc->min_spatial_segmentation_idc = MAX_SPATIAL_SEGMENTATION + 1;
}
