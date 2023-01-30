static int hvcc_array_add_nal_unit(uint8_t *nal_buf, uint32_t nal_size,
                                   uint8_t nal_type, int ps_array_completeness,
                                   HEVCDecoderConfigurationRecord *hvcc)
{
    int ret;
    uint8_t index;
    uint16_t numNalus;
    HVCCNALUnitArray *array;
    for (index = 0; index < hvcc->numOfArrays; index++)
        if (hvcc->array[index].NAL_unit_type == nal_type)
            break;
    if (index >= hvcc->numOfArrays) {
        uint8_t i;
        ret = av_reallocp_array(&hvcc->array, index + 1, sizeof(HVCCNALUnitArray));
        if (ret < 0)
            return ret;
        for (i = hvcc->numOfArrays; i <= index; i++)
            memset(&hvcc->array[i], 0, sizeof(HVCCNALUnitArray));
        hvcc->numOfArrays = index + 1;
    }
    array    = &hvcc->array[index];
    numNalus = array->numNalus;
    ret = av_reallocp_array(&array->nalUnit, numNalus + 1, sizeof(uint8_t*));
    if (ret < 0)
        return ret;
    ret = av_reallocp_array(&array->nalUnitLength, numNalus + 1, sizeof(uint16_t));
    if (ret < 0)
        return ret;
    array->nalUnit      [numNalus] = nal_buf;
    array->nalUnitLength[numNalus] = nal_size;
    array->NAL_unit_type           = nal_type;
    array->numNalus++;
    





    if (nal_type == NAL_VPS || nal_type == NAL_SPS || nal_type == NAL_PPS)
        array->array_completeness = ps_array_completeness;
    return 0;
}
