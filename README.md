# Netpbm-Sobel

The program allows the user to open an existing RGB image, 
convert it to grayscale, apply the Sobel operator to 
the image and then save it to a file. Once again, the resulting
file will contain the image in the grayscale format.

Even though, the task only includes what is described above,
the codebase has a lot more potential. It allows to open, read
and write to disk any type of Netpbm image: P1..P6.

## Building

The project does not have any external dependencies, therefore,
to compile the project you just need `gcc` and `make`.

In order to build the executable, run `make` or `make netpbm-sobel` in the
root folder of the project.

The binary can then be found at `%PROJECT%/build/netpbm-sobel`.

## Running

The program must be executed with the following command line
arguments: `./netpbm-sobel "source_image_path" "new_image_path" [threads]`,
where `threads` is a number of threads to use. This field is optional,
if it is omitted, 1 thread is used.

## Notes

This project was implemented as a test task for my internship application
at OMP (Sailfish Mobile OS RUS).

