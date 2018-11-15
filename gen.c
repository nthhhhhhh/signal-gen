#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

bool is_little_endian(void);

int main(int argc, char* argv[argc +1])
{
    enum {
        ARG_NAME,
        ARG_OUTFILE,
        ARG_DUR,
        ARG_HZ,
        ARG_SR,
        ARG_AMP,
        ARG_TYPE,
        ARG_NARGS
    };


    enum samptype {
        RAWSAMP_SHORT,
        RAWSAMP_FLOAT
    };


    if (argc != ARG_NARGS) {
        printf("Usage: tfork outfile.txt dur freq srate slope\n");
        return EXIT_FAILURE;
    }


    FILE* fp =
        fopen( argv[ARG_OUTFILE]
             , "wb" );


    if (fp == NULL) {
        fprintf( stderr
               , "Error creating output file %s\n"
               , argv[ARG_OUTFILE]);

        return EXIT_FAILURE;
    }


    #ifndef M_PI
    const double M_PI =
        3.141592654;
    #endif

    const unsigned MAXFRAME =
        {0};

    const double TWO_PI =
        2.0 * M_PI;

    const double DUR =
        atof(argv[ARG_DUR]);

    const double FREQ =
        atof(argv[ARG_HZ]);

    const double SRATE =
        atof(argv[ARG_SR]);

    const double AMP =
        atof(argv[ARG_AMP]);

    const unsigned SAMP_TYPE =
        (unsigned) atoi(argv[ARG_TYPE]);

    if (SAMP_TYPE != 0 || SAMP_TYPE != 1) {
        printf("error: sampletype can "
               "only be 0 or 1\n");

        return EXIT_FAILURE;
    }

    const char* ENDIANNESS[2] =
        { "big_endian", "little_endian" };

    const unsigned NUM_SAMPLES =
        (unsigned) (DUR * SRATE);

    const double ANGLE_INC =
        TWO_PI * FREQ / NUM_SAMPLES;

    double start =
        1.0;

    const double END =
        1.0e-4; // -80dB

    double maxsamp =
        0.0;

    const double FAC =
        pow( END / start
           , 1.0 / NUM_SAMPLES);

    const bool IS_L_ENDIAN =
        is_little_endian();

    printf( "Writing %d %s samples\n"
          , NUM_SAMPLES
          , ENDIANNESS[IS_L_ENDIAN]);

    /* run loop for this type of sample. */
    if(SAMP_TYPE == RAWSAMP_SHORT) {
        for(unsigned i = 0; i < NUM_SAMPLES; ++i) {
            const double SAMPLE =
                start * AMP * sin(ANGLE_INC * i);

            start =
                start * FAC;

            /* use 32767 to avoid overflow problem */
            const short SSAMP =
                (short) (SAMPLE * 32767.0);

            /* file used to write into buffer */
            size_t filestatus =
                fwrite(&SSAMP, sizeof(short), 1, fp);

            if(filestatus != 1) {
                printf("Error writing data to file\n");
                return EXIT_FAILURE;
            }

            maxsamp = fabs(SAMPLE) > maxsamp
                    ? fabs(SAMPLE)
                    : maxsamp;
        }
    } else {
        for(unsigned i = 0; i < NUM_SAMPLES; ++i) {
            const double SAMPLE =
                start * AMP * sin(ANGLE_INC * i);

            start =
                start * FAC;

            const float FSAMP =
                (float) SAMPLE;

            /* file used to write into buffer */
            size_t filestatus =
                fwrite(&FSAMP, sizeof(short), 1, fp);

            if(filestatus != 1) {
                printf("Error writing data to file\n");
                return EXIT_FAILURE;
            }

            maxsamp = fabs(SAMPLE) > maxsamp
                    ? fabs(SAMPLE)
                    : maxsamp;

        }
    }

    fclose(fp);

    printf( "done. Maximum sample value = %.81f at frame %d\n"
          , maxsamp
          , MAXFRAME);

    return EXIT_SUCCESS;

}

bool is_little_endian(void)
{
    /* full length memory with value of 1 */
    size_t oneatfullsize =
        1;

    /* smaller space to truncate oneatfullsize */
    uint8_t *shortersize = 
        (uint8_t *) &oneatfullsize;

    bool islilendian = *shortersize > 0
                ? true
                : false;

    return islilendian;

}

