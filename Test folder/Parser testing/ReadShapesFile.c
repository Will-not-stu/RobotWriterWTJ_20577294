#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>             // For string manipulation functions
#include "ReadShapesFile.h"     // Might want to include stdint.h

int ReadShapesFile(const char *FileName, Shape **shapes, int *numShapes) // ** allows function to allocate memory and update shape pointer in main.c
{                                                                           /* All structs should be defined in the header file */
    FILE *file = NULL;                                                          // Initialize file pointer to NULL
    char Header [50], ExtraData;                                                // Buffer to hold the header line
    int i, j;      

/* Opening of the file */   
    file = fopen(FileName, "r");

/* Check that the file opens correctly */
    if (file == NULL)
    {
        printf("Error: Unable to open the file provided\nPlease make sure the filename is correct and try again!\n");
        return 1;
    }

/* Checking header of text file */
    int HeaderResult = fscanf(file, "%49s %d", Header, numShapes);      // %49s is the length of the character array (remembering index 0)
                                                                        // Extract Header name and number of shapes in the file, 2 indicates succesful reading of both parts of the header
    switch(HeaderResult)
        {
            case EOF:                                                   // EOF returns -1
                printf("Error: Empty file or file reading failure\n");
                fclose(file);
                return 1;
            case 0:
                printf("Error: Unable to read file header\n");
                fclose(file);
                return 1;
            case 1:
                printf("Error: Missing or invalid number of shapes in header\n");
                fclose(file);
                return 1;
            case 2:
                break;
            default:
                printf("Error: An unexpected error has occurred while reading the file\n");
                fclose(file);
                return 1;
        }

/* Check that the file header is as expected and that it says there is shapes in the file */    
    if (strcmp(Header, "NumShapes") != 0)
        {
            printf("Error: File header is of an incorrect format\n");
            fclose(file);
            return 1;
        }
    if (*numShapes < 1)
        {
            printf("Error: There is an invalid number of shapes in this file\n");
            fclose(file);
            return 1;
        }  

    printf("Success: File header read correctly\n");

/* Allocate memory for the amount of shapes with the size in bytes of their Shape structure */
    *shapes = calloc(*numShapes, sizeof(Shape));                         
                                                                        // Memory has been made for the shapes but not the stroke data
    if (*shapes == NULL)
        {
            printf("Error: Shape memory allocation failed\n");
            FreeShapeData(shapes, numShapes);
            fclose(file);
            return 1;
        }

/* Setting up a loop to extract all the shape names and the number of strokes each shape has */
    for (i = 0; i < *numShapes; i++)
        {
            HeaderResult = fscanf(file, "%46s %d", (*shapes)[i].shapeName, &(*shapes)[i].numPoints); // First one accesses current shape name in the allocated array and the second one accesses the ADDRESS of the number of points for that shape
            switch(HeaderResult)
                {
                    case EOF:                                                   
                        printf("Error: No shape data or file reading failure\n");
                        FreeShapeData(shapes, numShapes);
                        fclose(file);
                        return 1;
                    case 0:
                        printf("Error: Unable to read shape name\n");
                        FreeShapeData(shapes, numShapes);
                        fclose(file);
                        return 1;
                    case 1:
                        printf("Error: Missing or invalid number of points in shape data\n");
                        FreeShapeData(shapes, numShapes);
                        fclose(file);
                        return 1;
                    case 2:
                        break;
                    default:
                        printf("Error: An unexpected error has occurred while reading the shape data\n");
                        FreeShapeData(shapes, numShapes);
                        fclose(file);
                        return 1;
                }

/* Check that there is enough points to draw a shape not a line */
            if ((*shapes)[i].numPoints < 3)
                {
                    printf("Error: There is not a sufficient amount of points to draw a shape\n");
                    FreeShapeData(shapes, numShapes);
                    fclose(file);
                    return 1;
                }

 /* Allocating an array for the stroke data with numPoint indexes the size of the Point structure */           
            (*shapes)[i].points = calloc((*shapes)[i].numPoints, sizeof(Point));     // Code is grey due to Point not being defined in the header file
            if((*shapes)[i].points == NULL)
                {
                    printf("Error: Stroke data memory allocation failed for the %s", (*shapes)[i].shapeName);
                    FreeShapeData(shapes, numShapes);
                    fclose(file);
                    return 1;
                }

/* Setting up a loop to extract the stroke data */
            for (j = 0; j < (*shapes)[i].numPoints; j++)
                {
                    int PointData = fscanf(file, "%f %f %d", &(*shapes)[i].points[j].x, &(*shapes)[i].points[j].y, &(*shapes)[i].points[j].penState);

/* Check that the data exists and is of the correct data type */
                    switch(PointData)
                        {
                            case EOF:
                                printf("Error: No stroke data or file reading failure\n");                                              // Unexpected EOF detetcted
                                FreeShapeData(shapes, numShapes);
                                fclose(file);
                                return 1;
                            case 0:
                                printf("Error: Invalid x co-ordinate in shape %s, point %d\n", (*shapes)[i].shapeName, j + 1);          // Checking for correct data type and that data is there as expected
                                FreeShapeData(shapes, numShapes);
                                fclose(file);
                                return 1;
                            case 1:
                                printf("Error: Invalid y co-ordinate in shape %s, point %d\n", (*shapes)[i].shapeName, j + 1);          // Checking for correct data type and that data is there as expected
                                FreeShapeData(shapes, numShapes);
                                fclose(file);
                                return 1;
                            case 2:
                                printf("Error: Invalid penState in shape %s, point %d\n", (*shapes)[i].shapeName, j + 1);              // Checking for correct data type and that data is there as expected
                                FreeShapeData(shapes, numShapes);
                                fclose(file);
                                return 1;
                            case 3:                                                                                                     // All data read successfully as expected
                                break;
                            default:
                                printf("Error: An unexpected error has occurred while reading the stroke data\n");
                                FreeShapeData(shapes, numShapes);
                                fclose(file);
                                return 1;
                        }

/* Now checking that the data read is within the correct ranges */
                    if ((*shapes)[i].points[j].penState != 0 && (*shapes)[i].points[j].penState != 1)                         // Checking for valid pen state "0" or "1"
                        {
                            printf("Error: Invalid pen state in shape %s, point %d.\n", (*shapes)[i].shapeName, j + 1);
                            FreeShapeData(shapes, numShapes);
                            fclose(file);
                            return 1;
                        }
                    if ((*shapes)[i].points[j].x < 0 || !isfinite((*shapes)[i].points[j].x))                                         // Checking for valid coordinates >= 0 and not infinite
                        {
                            printf("Error: Invalid x co-ordinate in shape %s, point %d\n", (*shapes)[i].shapeName, j + 1);
                            FreeShapeData(shapes, numShapes);
                            fclose(file);
                            return 1;
                        }
                    if ((*shapes)[i].points[j].y < 0 || !isfinite((*shapes)[i].points[j].y))                                         // Checking for valid coordinates >= 0 and not infinite
                        {
                            printf("Error: Invalid y co-ordinate in shape %s, point %d\n", (*shapes)[i].shapeName, j + 1);
                            FreeShapeData(shapes, numShapes);
                            fclose(file);
                            return 1;
                        }
                }

            printf("Success: Stroke data for %s read successfully!\n", (*shapes)[i].shapeName);
        }

/* Check for any extra data excluding blank lines or file reading errors */
        if (fscanf(file, " %c", &ExtraData) != EOF)             // SPACE %c does not cause an error to be triggered for spaces, white lines or tabs
        {
            printf("Error: Unexpected extra data has been detected in the file!\n");
            FreeShapeData(shapes, numShapes);
            fclose(file);
            return 1;
        }

        if (ferror(file))                                       // Returns "1" if there has been an error reading the file
        {
            printf("Error: File reading error has occurred!\n");
            FreeShapeData(shapes, numShapes);                                      // Could just implement a free memory function
            fclose(file);
            return 1;
        }

fclose(file);
printf("Success: File was opened, data was read and data was stored all successfully\n");

/*
4. Create the FreeShapes() function.
5. Implement memory cleanup for every error condition.
7. Test the complete function
*/

    return 0; // Return 0 to indicate success
}

void FreeShapeData(Shape **shapes, int *numShapes)
{
    int i;

    /* Check if there is memory to release */
    if (*shapes != NULL)
        {
            for (i = 0; i < *numShapes; i++)
                {
                    free((*shapes)[i].points);              // free the stroke data for each shape
                    (*shapes)[i].points = NULL;             // Make sure pointer is not dangling and points to NULL
                }
            free(*shapes);                                  // free the shapes
        }
    *shapes = NULL;                                 // Set all data to NULL to start again fresh
    *numShapes = 0;                              // numShapes could have a value without *shapes having anything in  
}