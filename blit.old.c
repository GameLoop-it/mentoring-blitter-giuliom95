#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const int ScreenWidth = 1024;
const int ScreenHeight = 768;
const int SpriteSize = 64;

void blitPixel(uint16_t* bufferPix, uint8_t* spritePix) {
    // Skip if pixel is completely transparent
    if(spritePix[3] == 0)
        return;
    
    // Avoid blending calculations if pixel is completely opaque
    if(spritePix[3] == 255) {
        bufferPix[0] =	(spritePix[2] << ((6+5)-3)	& 0b1111100000000000) | 
					    (spritePix[1] << (5-2)		& 0b0000011111100000) | 
					    (spritePix[0] >> 3			& 0b0000000000011111) ;
        return;
    }

	float over[3];
	float under[3];

    // Convert BGRA8888 to floatRGBA
	over[0] = spritePix[2] / (float)(255);
	over[1] = spritePix[1] / (float)(255);
	over[2] = spritePix[0] / (float)(255);

    // Convert RGB565 to floatRGB
	under[0] = ((bufferPix[0] & 0b1111100000000000) >> (6+5)) / (float)(31);
	under[1] = ((bufferPix[0] & 0b0000011111100000) >> 5)     / (float)(63);
	under[2] =  (bufferPix[0] & 0b0000000000011111)           / (float)(31);

    // Perform alpha blending
	float a = spritePix[3] / (float)(255);
	float b = 1-a;
	under[0] = b*under[0] + a*over[0];
	under[1] = b*under[1] + a*over[1];
	under[2] = b*under[2] + a*over[2];

    // Convert floatRGB to RGB565 and write to the buffer
	bufferPix[0] =	(uint16_t)(under[0]*31) << 11	|
					(uint16_t)(under[1]*63) << 5	|
					(uint16_t)(under[2]*31);
}


void blitSprite(uint16_t* frameBuffer, uint8_t* sprite, int x, int y) {
	const int spritePixNum = SpriteSize*SpriteSize;

	for(int i = 0; i < SpriteSize; ++i) {
		int rowOffset = i * SpriteSize;
		for(int j = 0; j < SpriteSize; ++j) {
			int absX = x+j;
			int absY = y+i;
            // Check if the pixel is inside the buffer
			if(absX >= 0 && absX < ScreenWidth && absY >= 0 && absY < ScreenHeight) {
				blitPixel(frameBuffer + absY*ScreenWidth + absX, sprite + 4*(j + rowOffset));
			}
		}
	}
}

void buildLUTs() {}