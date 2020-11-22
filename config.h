#define REFRESH_TIME 30
#define STARTPOS_Y  12
#define STARTPOS_X  0
#define LEFT_OFFSET 0
#define TOP_OFFSET 12
#define RIGHT_OFFSET 0
#define BOTTOM_OFFSET 0
#define BOX_HEIGHT_RATE 0.9
#define GAP_SIZE 3 

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
