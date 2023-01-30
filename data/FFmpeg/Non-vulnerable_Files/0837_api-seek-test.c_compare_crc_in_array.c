static int compare_crc_in_array(int64_t crc, int64_t pts)
{
    int i;
    for (i = 0; i < number_of_elements; i++) {
        if (pts_array[i] == pts) {
            if (crc_array[i] == crc) {
                printf("Comparing 0x%08lx %"PRId64" %d is OK\n", crc, pts, i);
                return 0;
            }
            else {
                av_log(NULL, AV_LOG_ERROR, "Incorrect crc of a frame after seeking\n");
                return -1;
            }
        }
    }
    av_log(NULL, AV_LOG_ERROR, "Incorrect pts of a frame after seeking\n");
    return -1;
}
