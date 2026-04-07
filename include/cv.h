#ifndef _CV_H_
#define _CV_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int width;
    int height;
    int channels;
    unsigned char* imageData;
} IplImage;

static inline IplImage* cvLoadImage(const char* filename, int flags) {
    printf("[cvLoadImage] Reading PGM image: %s\n", filename);

    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("Error: Cannot open file %s\n", filename);
        return NULL;
    }

    char header[3];
    int width, height, maxval;

    if (fscanf(f, "%2s", header) != 1) { fclose(f); return NULL; }
    if (strcmp(header, "P5") != 0) {
        printf("Error: Only binary PGM (P5) supported.\n");
        fclose(f);
        return NULL;
    }

    int c = fgetc(f);
    while (c == '#') {
        while (fgetc(f) != '\n');
        c = fgetc(f);
    }
    ungetc(c, f);

    if (fscanf(f, "%d %d %d", &width, &height, &maxval) != 3) {
        printf("Error reading header.\n");
        fclose(f);
        return NULL;
    }

    fgetc(f);

    IplImage* img = (IplImage*)malloc(sizeof(IplImage));
    img->width = width;
    img->height = height;
    img->channels = 1;
    img->imageData = (unsigned char*)malloc(width * height);

    fread(img->imageData, 1, width * height, f);
    fclose(f);

    printf("[cvLoadImage] Loaded %dx%d image successfully.\n", width, height);
    return img;
}

static inline int cvSaveImage(const char* filename, IplImage* img, const int* params) {
    printf("[cvSaveImage] Writing output to %s\n", filename);

    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("Error: Cannot save image to %s\n", filename);
        return 0;
    }

    fprintf(f, "P5\n%d %d\n255\n", img->width, img->height);
    fwrite(img->imageData, 1, img->width * img->height, f);
    fclose(f);
    return 1;
}

static inline void cvReleaseImage(IplImage** img) {
    if (img && *img) {
        free((*img)->imageData);
        free(*img);
        *img = NULL;
    }
}

static inline void cvNegative(IplImage* src, IplImage* dst) {
    printf("[cvNegative] Applying negative transformation...\n");
    int total = src->width * src->height * src->channels;
    for (int i = 0; i < total; i++) {
        dst->imageData[i] = 255 - src->imageData[i];
    }
}

static inline void cvBlur(IplImage* src, IplImage* dst) {

    int w = src->width;
    int h = src->height;
    unsigned char* s = src->imageData;
    unsigned char* d = dst->imageData;

    int kernel[25] = {
        1,4,6,4,1,
        4,16,24,16,4,
        6,24,36,24,6,
        4,16,24,16,4,
        1,4,6,4,1
    };

    for (int y = 2; y < h - 2; y++) {
        for (int x = 2; x < w - 2; x++) {

            int sum = 0;
            int idx = 0;

            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    sum += s[(y + ky) * w + (x + kx)] * kernel[idx++];
                }
            }

            d[y * w + x] = (unsigned char)(sum / 256);
        }
    }

    for (int x = 0; x < w; x++) {
        d[x] = s[x];
        d[(h - 1) * w + x] = s[(h - 1) * w + x];
    }

    for (int y = 0; y < h; y++) {
        d[y * w] = s[y * w];
        d[y * w + (w - 1)] = s[y * w + (w - 1)];
    }
}

#endif
