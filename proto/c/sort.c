// gcc -o sort.exe sort.c

#include <stdio.h>
 
unsigned char max (unsigned char *vals, unsigned char n, unsigned char i, unsigned char j, unsigned char k) {
    unsigned char m = i;
    if (j < n && vals[j] > vals[m]) {
        m = j;
    }
    if (k < n && vals[k] > vals[m]) {
        m = k;
    }
    return m;
}
 
void downheap (unsigned char *vals, unsigned char *idxs, unsigned char n, unsigned char i) {
    while (1) {
        unsigned char j = max(vals, n, i, 2 * i + 1, 2 * i + 2);
        if (j == i) {
            break;
        }
        unsigned char t = vals[i];
        vals[i] = vals[j];
        vals[j] = t;

        t = idxs[i];
        idxs[i] = idxs[j];
        idxs[j] = t;

        i = j;
    }
}
 
void heapsort (unsigned char *vals, unsigned char *idxs, unsigned char n) {
    signed char i;
    for (i = (n - 2) / 2; i >= 0; i--) {
        downheap(vals, idxs, n, i);
    }
    for (i = 0; i < n; i++) {
        unsigned char t = vals[n - i - 1];
        vals[n - i - 1] = vals[0];
        vals[0] = t;

        t = idxs[n-i-1];
        idxs[n - i - 1] = idxs[0];
        idxs[0] = t;

        downheap(vals, idxs, n - i - 1, 0);
    }
}
 
unsigned char main () {
    unsigned char vals[] = {4, 65, 2, 31, 0, 99, 2, 83, 255, 1};
    unsigned char idxs[] = {0, 1, 2, 3, 5, 7, 9, 11, 13, 15};
    unsigned char n = sizeof vals / sizeof vals[0];
    unsigned char i;
    for (i = 0; i < n; i++)
        printf("%d%s", idxs[i], i == n - 1 ? "\n" : "\t");
    for (i = 0; i < n; i++)
        printf("%d%s", vals[i], i == n - 1 ? "\n" : "\t");
    printf ("sorting\n");
    heapsort(vals, idxs, n);
    for (i = 0; i < n; i++)
        printf("%d%s", idxs[i], i == n - 1 ? "\n" : "\t");
    for (i = 0; i < n; i++)
        printf("%d%s", vals[i], i == n - 1 ? "\n" : "\t");
    printf ("fin\n");
    return 0;
}