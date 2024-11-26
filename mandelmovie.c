/******************************************************************************************************************
* @file mandelmovie.c
* @brief Multiprocessing 
*
* Course: CPE2600
* Section: 111
* Assignment: CPE Lab 11
* Author: Reagan Burkemper
*
* Description: This program generates 100 frames of the Mandelbrot
* Date: 11/20/2024
*
* Compile Instructions: 
*   To compile the mandelmovie, run:
*   - compile with: 
*       - make clean
*       - make
*   - run:
*       -./mandelmovie -p <process_count>
*   
*****************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
void help()
{
    printf("Help: \n");
    printf("Use: mandelmovie -p <process_count>\n");
    printf("Options:\n");
    printf("  -p <process_count>  Number of child processes to use (default=1)\n");
    printf("  -h                  Show help\n");
}

int main(int argc, char *argv[]) 
{
    int process_count = 1;  // default process count
    int frames = 100;        // number of frames to generate
    char c;

    // Parse command-line arguments
    while ((c = getopt(argc, argv, "p:h")) != -1) 
    {
        switch (c) 
        {
            case 'p':
                process_count = atoi(optarg);
                if (process_count < 1) //error checking to make sure its not 0
                {
                    fprintf(stderr, "Invalid: Process count must be at least 1.\n");
                    exit(1);
                }
                break;
            case 'h':
                help();
                break;
            default:
                fprintf(stderr, "Invalid option, '-h' for help.\n");
                exit(1);
        }
    }

    printf("Creating mandelmovie with %d processes.\n", process_count); //for debugging purposes

    int active_processes = 0;
    double xcenter = -0.088;  // chnaged xcenter
    double ycenter = 0.654;   // changed ycenter
    double scale = 0.01;      // changed starting scale

    for (int i = 0; i < frames; i++) 
    {
        
        if (active_processes >= process_count) 
        {
            // wait for a child process to finish before creating a new one
            wait(NULL);
            active_processes--;
        }

        pid_t pid = fork();
        if (pid == 0) 
        {
            // run child process
            char outfile[256];
            snprintf(outfile, sizeof(outfile), "mandel%03d.jpg", i);

            char x_arg[64], y_arg[64], scale_arg[64], max_arg[64], width_arg[64], height_arg[64], outfile_arg[512];
            snprintf(x_arg, sizeof(x_arg), "-x%lf", xcenter);
            snprintf(y_arg, sizeof(y_arg), "-y%lf", ycenter);
            snprintf(scale_arg, sizeof(scale_arg), "-s%lf", scale);
            snprintf(max_arg, sizeof(max_arg), "-m1000");
            snprintf(width_arg, sizeof(width_arg), "-W1000");
            snprintf(height_arg, sizeof(height_arg), "-H1000");
            snprintf(outfile_arg, sizeof(outfile_arg), "-o%s", outfile);

            if (execl("./mandel", "mandel", x_arg, y_arg, scale_arg, max_arg, width_arg, height_arg, outfile_arg, (char *)NULL) == -1) 
            {
                perror("execl failed");
                exit(1);
            }

            execl("./mandel", "mandel", x_arg, y_arg, scale_arg, max_arg, width_arg, height_arg, outfile_arg, (char *)NULL);

            // If execl fails
            //perror("execl failed");
            //exit(1);
        } else if (pid > 0) 
        {
            // parent process
            active_processes++;
            
            scale *= 0.95;  // changed zoom (reducing scale)
            //xcenter += 0.01;  // shift in x direction
            //ycenter -= 0.01;  // shift in y direction

        } else //error checking
        {
            perror("fork failed");
            exit(1);
        }
    }

    while (active_processes > 0)     // wait for processes to finish
    {
        wait(NULL);
        active_processes--;
    }
    printf("All frames generated. Creating movie.\n"); //debugging purposes

    // stitch frames into a movie using ffmpeg
    if (system("ffmpeg -i mandel%03d.jpg mandelmovie.mpg") != 0) 
    {
        //error checking
        fprintf(stderr, "Failed to create movie using ffmpeg.\n");
        exit(1);
    }

    printf("Movie created: mandelmovie1.mpg\n");
    return 0;

}