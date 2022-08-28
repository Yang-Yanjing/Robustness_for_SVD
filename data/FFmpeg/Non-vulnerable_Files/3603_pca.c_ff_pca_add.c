void ff_pca_add(PCA *pca, const double *v){
    int i, j;
    const int n= pca->n;
    for(i=0; i<n; i++){
        pca->mean[i] += v[i];
        for(j=i; j<n; j++)
            pca->covariance[j + i*n] += v[i]*v[j];
    }
    pca->count++;
}
