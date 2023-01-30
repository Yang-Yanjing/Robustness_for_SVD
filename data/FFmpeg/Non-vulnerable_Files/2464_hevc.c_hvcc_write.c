static int hvcc_write(AVIOContext *pb, HEVCDecoderConfigurationRecord *hvcc)
{
    uint8_t i;
    uint16_t j, vps_count = 0, sps_count = 0, pps_count = 0;
    


    hvcc->configurationVersion = 1;
    


    if (hvcc->min_spatial_segmentation_idc > MAX_SPATIAL_SEGMENTATION)
        hvcc->min_spatial_segmentation_idc = 0;
    




    if (!hvcc->min_spatial_segmentation_idc)
        hvcc->parallelismType = 0;
    



    hvcc->avgFrameRate      = 0;
    hvcc->constantFrameRate = 0;
    av_log(NULL, AV_LOG_TRACE,  "configurationVersion:                %"PRIu8"\n",
            hvcc->configurationVersion);
    av_log(NULL, AV_LOG_TRACE,  "general_profile_space:               %"PRIu8"\n",
            hvcc->general_profile_space);
    av_log(NULL, AV_LOG_TRACE,  "general_tier_flag:                   %"PRIu8"\n",
            hvcc->general_tier_flag);
    av_log(NULL, AV_LOG_TRACE,  "general_profile_idc:                 %"PRIu8"\n",
            hvcc->general_profile_idc);
    av_log(NULL, AV_LOG_TRACE, "general_profile_compatibility_flags: 0x%08"PRIx32"\n",
            hvcc->general_profile_compatibility_flags);
    av_log(NULL, AV_LOG_TRACE, "general_constraint_indicator_flags:  0x%012"PRIx64"\n",
            hvcc->general_constraint_indicator_flags);
    av_log(NULL, AV_LOG_TRACE,  "general_level_idc:                   %"PRIu8"\n",
            hvcc->general_level_idc);
    av_log(NULL, AV_LOG_TRACE,  "min_spatial_segmentation_idc:        %"PRIu16"\n",
            hvcc->min_spatial_segmentation_idc);
    av_log(NULL, AV_LOG_TRACE,  "parallelismType:                     %"PRIu8"\n",
            hvcc->parallelismType);
    av_log(NULL, AV_LOG_TRACE,  "chromaFormat:                        %"PRIu8"\n",
            hvcc->chromaFormat);
    av_log(NULL, AV_LOG_TRACE,  "bitDepthLumaMinus8:                  %"PRIu8"\n",
            hvcc->bitDepthLumaMinus8);
    av_log(NULL, AV_LOG_TRACE,  "bitDepthChromaMinus8:                %"PRIu8"\n",
            hvcc->bitDepthChromaMinus8);
    av_log(NULL, AV_LOG_TRACE,  "avgFrameRate:                        %"PRIu16"\n",
            hvcc->avgFrameRate);
    av_log(NULL, AV_LOG_TRACE,  "constantFrameRate:                   %"PRIu8"\n",
            hvcc->constantFrameRate);
    av_log(NULL, AV_LOG_TRACE,  "numTemporalLayers:                   %"PRIu8"\n",
            hvcc->numTemporalLayers);
    av_log(NULL, AV_LOG_TRACE,  "temporalIdNested:                    %"PRIu8"\n",
            hvcc->temporalIdNested);
    av_log(NULL, AV_LOG_TRACE,  "lengthSizeMinusOne:                  %"PRIu8"\n",
            hvcc->lengthSizeMinusOne);
    av_log(NULL, AV_LOG_TRACE,  "numOfArrays:                         %"PRIu8"\n",
            hvcc->numOfArrays);
    for (i = 0; i < hvcc->numOfArrays; i++) {
        av_log(NULL, AV_LOG_TRACE, "array_completeness[%"PRIu8"]:               %"PRIu8"\n",
                i, hvcc->array[i].array_completeness);
        av_log(NULL, AV_LOG_TRACE, "NAL_unit_type[%"PRIu8"]:                    %"PRIu8"\n",
                i, hvcc->array[i].NAL_unit_type);
        av_log(NULL, AV_LOG_TRACE, "numNalus[%"PRIu8"]:                         %"PRIu16"\n",
                i, hvcc->array[i].numNalus);
        for (j = 0; j < hvcc->array[i].numNalus; j++)
            av_log(NULL, AV_LOG_TRACE,
                    "nalUnitLength[%"PRIu8"][%"PRIu16"]:                 %"PRIu16"\n",
                    i, j, hvcc->array[i].nalUnitLength[j]);
    }
    


    for (i = 0; i < hvcc->numOfArrays; i++)
        switch (hvcc->array[i].NAL_unit_type) {
        case NAL_VPS:
            vps_count += hvcc->array[i].numNalus;
            break;
        case NAL_SPS:
            sps_count += hvcc->array[i].numNalus;
            break;
        case NAL_PPS:
            pps_count += hvcc->array[i].numNalus;
            break;
        default:
            break;
        }
    if (!vps_count || vps_count > MAX_VPS_COUNT ||
        !sps_count || sps_count > MAX_SPS_COUNT ||
        !pps_count || pps_count > MAX_PPS_COUNT)
        return AVERROR_INVALIDDATA;
    
    avio_w8(pb, hvcc->configurationVersion);
    




    avio_w8(pb, hvcc->general_profile_space << 6 |
                hvcc->general_tier_flag     << 5 |
                hvcc->general_profile_idc);
    
    avio_wb32(pb, hvcc->general_profile_compatibility_flags);
    
    avio_wb32(pb, hvcc->general_constraint_indicator_flags >> 16);
    avio_wb16(pb, hvcc->general_constraint_indicator_flags);
    
    avio_w8(pb, hvcc->general_level_idc);
    



    avio_wb16(pb, hvcc->min_spatial_segmentation_idc | 0xf000);
    



    avio_w8(pb, hvcc->parallelismType | 0xfc);
    



    avio_w8(pb, hvcc->chromaFormat | 0xfc);
    



    avio_w8(pb, hvcc->bitDepthLumaMinus8 | 0xf8);
    



    avio_w8(pb, hvcc->bitDepthChromaMinus8 | 0xf8);
    
    avio_wb16(pb, hvcc->avgFrameRate);
    





    avio_w8(pb, hvcc->constantFrameRate << 6 |
                hvcc->numTemporalLayers << 3 |
                hvcc->temporalIdNested  << 2 |
                hvcc->lengthSizeMinusOne);
    
    avio_w8(pb, hvcc->numOfArrays);
    for (i = 0; i < hvcc->numOfArrays; i++) {
        




        avio_w8(pb, hvcc->array[i].array_completeness << 7 |
                    hvcc->array[i].NAL_unit_type & 0x3f);
        
        avio_wb16(pb, hvcc->array[i].numNalus);
        for (j = 0; j < hvcc->array[i].numNalus; j++) {
            
            avio_wb16(pb, hvcc->array[i].nalUnitLength[j]);
            
            avio_write(pb, hvcc->array[i].nalUnit[j],
                       hvcc->array[i].nalUnitLength[j]);
        }
    }
    return 0;
}
