# RTOS-Image-Processing-QNX

## Overview

This project demonstrates multithreaded image processing using POSIX threads on a QNX RTOS environment.

It implements a complete pipeline including image loading, negative transformation, and Gaussian blurring using synchronized threads.


## Features

* Multithreading using pthreads
* Synchronization using mutex and condition variables
* Image loading and saving (PGM format)
* Negative image transformation
* Gaussian blur using a 5×5 kernel
* Sequential pipeline execution using thread coordination


## Architecture

* Thread 1 → Loads image
* Thread 2 → Performs negative transformation
* Thread 3 → Performs Gaussian blur


## How to Run

1. Use QNX Momentics IDE or a POSIX-compatible environment
2. Place input image at `/tmp/input1.pgm`
3. Compile and run the program


## My Contribution

* Designed and implemented the complete multithreaded image processing pipeline
* Implemented synchronization using mutex and condition variables
* Developed image processing functions (negative and Gaussian blur)
* Tested the system on QNX RTOS
