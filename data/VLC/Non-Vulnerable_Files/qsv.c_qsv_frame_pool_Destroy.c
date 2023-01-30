 */
static void qsv_frame_pool_Destroy(qsv_frame_pool_t *pool)
{
    for (size_t i = 0; i < pool->size; i++) {
        picture_t *pic = (picture_t *) pool->frames[i].Data.MemId;
        if (pic)
            picture_Release(pic);
    }
    free(pool->frames);
}
