 */
static void XCB_picture_Destroy (picture_t *pic)
{
    shmdt (pic->p[0].p_pixels);
    free (pic);
}
