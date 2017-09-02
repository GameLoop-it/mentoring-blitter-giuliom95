#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RGB565_R_MASK 0b1111100000000000
#define RGB565_G_MASK 0b0000011111100000
#define RGB565_B_MASK 0b0000000000011111

const int ScreenWidth = 1024;
const int ScreenHeight = 768;
const int SpriteSize = 64;

int min(int x, int y) {
    return x < y ? x : y;
}
int max(int x, int y) {
    return x > y ? x : y;
}

void bgra8888_to_floatrgba(uint8_t* bgra8888, float* floatrgba) {
    floatrgba[0] = bgra8888[2] * (1 / 255.f);
    floatrgba[1] = bgra8888[1] * (1 / 255.f);
    floatrgba[2] = bgra8888[0] * (1 / 255.f);
    floatrgba[3] = bgra8888[3] * (1 / 255.f);
}

void rgb565_to_floatrgb(uint16_t* rgb565, float* floatrgb) {
    floatrgb[0] = ((rgb565[0] & RGB565_R_MASK) >> (6+5))  * (1 / 31.f);
    floatrgb[1] = ((rgb565[0] & RGB565_G_MASK) >> 5)      * (1 / 63.f);
    floatrgb[2] =  (rgb565[0] & RGB565_B_MASK)            * (1 / 31.f);
}

float lerp(float from, float to, float time) {
    return (1-time)*from + time*to;
}

void floatrgb_to_rgb565(float* floatrgb, uint16_t* rgb565) {
    rgb565[0] = (uint16_t)(floatrgb[0]*31) << 11 |
                (uint16_t)(floatrgb[1]*63) << 5  |
                (uint16_t)(floatrgb[2]*31);
}

void blitSprite(uint16_t* frameBuffer, uint8_t* sprite, int x, int y) {
    int start_i = max(-y, 0);
    int start_j = max(-x, 0);
    int end_i = min(ScreenHeight-y, SpriteSize);
    int end_j = min(ScreenWidth-x , SpriteSize);

    int spriteRowOffset = 4*(SpriteSize - (end_j-start_j));
    int bufferRowOffset = ScreenWidth - (end_j-start_j);

    uint8_t* spritePix = sprite + 4*(start_j + start_i*SpriteSize);
    uint16_t* bufferPix = frameBuffer + ScreenWidth*(y + start_i) + x + start_j;

    for(int i = start_i; i < end_i; ++i) {

        for(int j = start_j; j < end_j; ++j) {

            float over[4];
            float under[3];

            bgra8888_to_floatrgba(spritePix, over);
            rgb565_to_floatrgb(bufferPix, under);
            
            // Perform alpha blending
            under[0] = lerp(under[0], over[0], over[3]);
            under[1] = lerp(under[1], over[1], over[3]);
            under[2] = lerp(under[2], over[2], over[3]);

            floatrgb_to_rgb565(under, bufferPix);

            spritePix += 4;
            bufferPix += 1;
        }

        spritePix += spriteRowOffset;
        bufferPix += bufferRowOffset;
    }
}

void buildLUTs() {}
