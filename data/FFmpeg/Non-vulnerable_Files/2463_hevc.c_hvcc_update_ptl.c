static void hvcc_update_ptl(HEVCDecoderConfigurationRecord *hvcc,
                            HVCCProfileTierLevel *ptl)
{
    



    hvcc->general_profile_space = ptl->profile_space;
    




    if (hvcc->general_tier_flag < ptl->tier_flag)
        hvcc->general_level_idc = ptl->level_idc;
    else
        hvcc->general_level_idc = FFMAX(hvcc->general_level_idc, ptl->level_idc);
    



    hvcc->general_tier_flag = FFMAX(hvcc->general_tier_flag, ptl->tier_flag);
    













    hvcc->general_profile_idc = FFMAX(hvcc->general_profile_idc, ptl->profile_idc);
    



    hvcc->general_profile_compatibility_flags &= ptl->profile_compatibility_flags;
    



    hvcc->general_constraint_indicator_flags &= ptl->constraint_indicator_flags;
}
