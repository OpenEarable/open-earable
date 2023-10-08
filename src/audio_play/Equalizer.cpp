#include "Equalizer.h"

#include <utility>

const float a[EQ_ORDER][3] = {{1,	-1.88005337532644,	0.886850855731273},
                        {1,	0.383474723870624,	0.240950274098301},
                        {1,	0.320433065251179,	0}};

const float b[EQ_ORDER][3] = {{1.13762426437059,	-1.84946471350470,	0.779815253182425},
                        {0.668254486486803,	0.383474723870624,	0.572695787611498},
                        {0.720639527015944,	0.599793538235235,	0}};


Equalizer::Equalizer() {
    //reset();
}

Equalizer::~Equalizer() {
    //reset();
}

void Equalizer::reset() {
    #pragma unroll
    for(int i = 0; i < EQ_ORDER; i++) {
        buffer[i][0] = 0;
        buffer[i][1] = 0;
    }
}

void Equalizer::update(int16_t * data, int length) {
    float y[EQ_ORDER+1] = {0};
    for (int n = 0; n < length; n++) {
        y[0] = data[n];

        #pragma unroll
        for (int k = 0; k < EQ_ORDER; k++) {
            y[k+1] = b[k][0] * y[k] + buffer[k][0];
            //#if (a[k][2] != 0 || b[k][2] == 0)
            buffer[k][0] = b[k][1] * y[k] - a[k][1] * y[k+1] + buffer[k][1];
            buffer[k][1] = b[k][2] * y[k] - a[k][2] * y[k+1];
            /*#else
            buffer[k][0] = b[k][1] * y[k] - a[k][1] * y[k+1];
            //buffer[k][1] = 0;
            #endif*/
        }

        //if (abs(0.095 * y[EQ_ORDER]) > (1<<15)-1) Serial.println("Limit!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

        data[n] = constrain(0.095 * y[EQ_ORDER],-1*(1<<15),(1<<15)-1);
    }
}