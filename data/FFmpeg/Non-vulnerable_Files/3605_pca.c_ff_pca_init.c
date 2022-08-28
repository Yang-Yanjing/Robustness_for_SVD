PCA *ff_pca_init(int n){
    PCA *pca;
    if(n<=0)
        return NULL;
    pca= av_mallocz(sizeof(*pca));
    if (!pca)
        return NULL;
    pca->n= n;
    pca->z = av_malloc_array(n, sizeof(*pca->z));
    pca->count=0;
    pca->covariance= av_calloc(n*n, sizeof(double));
    pca->mean= av_calloc(n, sizeof(double));
    if (!pca->z || !pca->covariance || !pca->mean) {
        ff_pca_free(pca);
        return NULL;
    }
    return pca;
}
