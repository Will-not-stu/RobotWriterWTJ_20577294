#ifndef ReadShapesFile_H                  // Checks function has not already been defined
#define ReadShapesFile_H                  

    typedef struct                  // Stroke data of one movement instruction
    {
        float x, y;
        int penState;
    } Point;

    typedef struct                  // Shape data of one shape
    {
        char shapeName[47];         // 47 Characters covers names up to a 65,537 sided polygon
        int numPoints;              // Number of strokes in the shape data
        Point *points;              // Pointer to a dynamically allocated array of Point structures
    } Shape;
    
    int ReadShapesFile(const char *FileName, Shape **shapes, int *numShapes); // Function prototype

#endif