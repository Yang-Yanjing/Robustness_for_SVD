static void set_identity_matrix(LUT3DContext *lut3d, int size)
{
    int i, j, k;
    const float c = 1. / (size - 1);
    lut3d->lutsize = size;
    for (k = 0; k < size; k++) {
        for (j = 0; j < size; j++) {
            for (i = 0; i < size; i++) {
                struct rgbvec *vec = &lut3d->lut[k][j][i];
                vec->r = k * c;
                vec->g = j * c;
                vec->b = i * c;
            }
        }
    }
}
