#include <stdio.h>
#include <stdlib.h>
#include "ReadShapesFile.h"

int main(void)
{
    Shape *shapes = NULL;
    int numShapes = 0;
    int result;

    result = ReadShapesFile("VariedShapeStrokeData.txt", &shapes, &numShapes);               // Change file name to appropriate txt file

    if (result == 0)
    {
        printf("Number of shapes: %d\n", numShapes);

        for (int i = 0; i < numShapes; i++)
        {
            printf("Shape: %s\n", shapes[i].shapeName);
            printf("Number of points: %d\n", shapes[i].numPoints);

            for (int j = 0; j < shapes[i].numPoints; j++)
            {
                printf("X: %.2f, Y: %.2f, Pen: %d\n", shapes[i].points[j].x, shapes[i].points[j].y, shapes[i].points[j].penState);
            }
        }
    }
    else
    {
        printf("Parser returned an error.\n");
    }

    // Release memory after testing
    if (shapes != NULL)
    {
        for (int i = 0; i < numShapes; i++)
        {
            free(shapes[i].points);
        }

        free(shapes);
    }

    return result;
}
