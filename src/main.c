#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "cv.h"

IplImage* input = NULL;
IplImage* outputNeg = NULL;
IplImage* outputBlur = NULL;

pthread_mutex_t img_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

int img_ready = 0;
int img_blur_start = 0;

void* imageLoader(void* arg) {

    printf("[Thread 1] Loading image...\n");

    pthread_mutex_lock(&img_mutex);

    input = cvLoadImage("/tmp/input1.pgm", 1);

    if (!input) {
        printf("[Thread 1] Failed to load image.\n");
        pthread_mutex_unlock(&img_mutex);
        return NULL;
    }

    img_ready = 1;
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&img_mutex);

    printf("[Thread 1] Image loaded successfully.\n");
    return NULL;
}

void* imageProcessorNeg(void* arg) {

    pthread_mutex_lock(&img_mutex);

    while (img_ready == 0) {
        pthread_cond_wait(&cond, &img_mutex);
    }

    if (!input) {
        pthread_mutex_unlock(&img_mutex);
        return NULL;
    }

    printf("[Thread 2] Starting image processing (Negative)...\n");

    outputNeg = (IplImage*)malloc(sizeof(IplImage));
    outputNeg->width = input->width;
    outputNeg->height = input->height;
    outputNeg->channels = input->channels;
    outputNeg->imageData = (unsigned char*)malloc(input->width * input->height * input->channels);

    cvNegative(input, outputNeg);

    cvSaveImage("/tmp/negative_output.pgm", outputNeg, 0);

    printf("[Thread 2] Processing complete.\n");

    img_blur_start = 1;
    pthread_cond_signal(&cond2);

    pthread_mutex_unlock(&img_mutex);
    return NULL;
}

void* imageProcessorBlur(void* arg) {

    pthread_mutex_lock(&img_mutex);

    while (img_ready == 0) {
        pthread_cond_wait(&cond, &img_mutex);
    }

    while (img_blur_start == 0) {
        pthread_cond_wait(&cond2, &img_mutex);
    }

    if (!input) {
        pthread_mutex_unlock(&img_mutex);
        return NULL;
    }

    printf("[Thread 3] Starting image processing (Blurring)...\n");

    outputBlur = (IplImage*)malloc(sizeof(IplImage));
    outputBlur->width = input->width;
    outputBlur->height = input->height;
    outputBlur->channels = input->channels;
    outputBlur->imageData = (unsigned char*)malloc(input->width * input->height * input->channels);

    for (int i = 0; i < 21; i++) {
        cvBlur(input, outputBlur);

        unsigned char* temp = input->imageData;
        input->imageData = outputBlur->imageData;
        outputBlur->imageData = temp;
    }

    cvSaveImage("/tmp/Blur_output.pgm", outputBlur, 0);

    printf("[Thread 3] Processing complete.\n");

    pthread_mutex_unlock(&img_mutex);
    return NULL;
}

int main() {

    pthread_t t1, t2, t3;

    printf("[Main] Starting RTOS Image Processing demo...\n");

    pthread_create(&t1, NULL, imageLoader, NULL);
    pthread_create(&t2, NULL, imageProcessorNeg, NULL);
    pthread_create(&t3, NULL, imageProcessorBlur, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    cvReleaseImage(&input);
    cvReleaseImage(&outputNeg);
    cvReleaseImage(&outputBlur);

    printf("[Main] Program completed successfully.\n");

    return 0;
}
