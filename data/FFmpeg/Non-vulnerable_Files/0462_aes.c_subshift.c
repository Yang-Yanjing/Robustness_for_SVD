static void subshift(av_aes_block s0[2], int s, const uint8_t *box)
{
    av_aes_block *s1 = (av_aes_block *) (s0[0].u8 - s);
    av_aes_block *s3 = (av_aes_block *) (s0[0].u8 + s);
    s0[0].u8[ 0] = box[s0[1].u8[ 0]];
    s0[0].u8[ 4] = box[s0[1].u8[ 4]];
    s0[0].u8[ 8] = box[s0[1].u8[ 8]];
    s0[0].u8[12] = box[s0[1].u8[12]];
    s1[0].u8[ 3] = box[s1[1].u8[ 7]];
    s1[0].u8[ 7] = box[s1[1].u8[11]];
    s1[0].u8[11] = box[s1[1].u8[15]];
    s1[0].u8[15] = box[s1[1].u8[ 3]];
    s0[0].u8[ 2] = box[s0[1].u8[10]];
    s0[0].u8[10] = box[s0[1].u8[ 2]];
    s0[0].u8[ 6] = box[s0[1].u8[14]];
    s0[0].u8[14] = box[s0[1].u8[ 6]];
    s3[0].u8[ 1] = box[s3[1].u8[13]];
    s3[0].u8[13] = box[s3[1].u8[ 9]];
    s3[0].u8[ 9] = box[s3[1].u8[ 5]];
    s3[0].u8[ 5] = box[s3[1].u8[ 1]];
}
