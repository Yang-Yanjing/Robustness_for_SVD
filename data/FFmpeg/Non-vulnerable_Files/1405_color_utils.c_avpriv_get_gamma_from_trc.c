double avpriv_get_gamma_from_trc(enum AVColorTransferCharacteristic trc)
{
    double gamma;
    switch (trc) {
        case AVCOL_TRC_BT709:
        case AVCOL_TRC_SMPTE170M:
        case AVCOL_TRC_SMPTE240M:
        case AVCOL_TRC_BT1361_ECG:
        case AVCOL_TRC_BT2020_10:
        case AVCOL_TRC_BT2020_12:
            

            gamma = 1.961;
            break;
        case AVCOL_TRC_GAMMA22:
        case AVCOL_TRC_IEC61966_2_1:
            gamma = 2.2;
            break;
        case AVCOL_TRC_GAMMA28:
            gamma = 2.8;
            break;
        case AVCOL_TRC_LINEAR:
            gamma = 1.0;
            break;
        default:
            gamma = 0.0; 
    }
    return gamma;
}
