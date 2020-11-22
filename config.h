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
