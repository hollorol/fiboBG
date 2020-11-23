#define REFRESH_TIME 30       // Refresh rate in seconds

#define STARTPOS_Y  12        // y coordinate of the top-left
                              //  corner of the background window
                        
#define STARTPOS_X  0         // x coordinate of the top-left
                              //  corner of the background window 
                              
#define BOX_HEIGHT_RATE 0.9   // The box height and screen height ratio
#define GAP_SIZE 3            // The gap size between the boxes

/* The clock well be centered inside of the defined box.
   The block bellow is for some slite modification. 
*/
#define LEFT_OFFSET 0          
#define TOP_OFFSET 12
#define RIGHT_OFFSET 0
#define BOTTOM_OFFSET 0

// +--------+---+-------------------
// +        +   +                  + 
// +        +---+                  +
// +        +   +                  +
// +--------+---+                  +
// +            +                  +
// +            +                  +
// +            +                  +
// +            +                  +
// +------------+------------------+
            
            

typedef struct _color {
    float red;
    float green;
    float blue;
} color;

// TIME COLORS
color colors[4]={
    //base
    {
        .red = 0.5,
        .green = 0.5,
        .blue = 0.5,     
    },

    //minute
    {
        .red = 1.,
        .green = 0.,
        .blue = 0.
    },

    //hour
    {
        .red = 0.,
        .green = 0.,
        .blue = 1
    }, 

    //both
    {
        .red = 0.,
        .green = 1.,
        .blue = 0.

    }

};
