#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "blit.new.c"

void ppm_to_rgb888(FILE* fd, uint8_t* buffer, const int w, const int h) {

	// Ignores PPM ASCII header
	int i = 0;
	char tmp;
	while(i<4) {
		fread(&tmp, 1, 1, fd);
		if(tmp == '\n')
			++i;
	}

	fread(buffer, 1, 3*w*h, fd);
}

void rgb888_to_ppm(FILE* fd, uint8_t* buffer, const int w, const int h) {
	fprintf(fd, "P6\n# TEST\n%d %d\n255\n", w, h);
	fwrite(buffer, 1, 3*w*h, fd);
}

void rgb888_to_rgb565(uint8_t* rgb888, uint16_t* rgb565, const int pixNum) {
	for(int i = 0; i < pixNum; ++i){
		rgb565[i] =	(rgb888[3*i]   >> 3) << 11 | 
					(rgb888[3*i+1] >> 2) << 5  | 
					(rgb888[3*i+2] >> 3);
	}
}


void rgb888_to_bgra8888(uint8_t* rgb888, uint8_t* bgra8888, const int pixNum, const uint8_t alpha) {
	for(int i = 0; i < pixNum; ++i) {
		bgra8888[4*i]   = rgb888[3*i+2];
		bgra8888[4*i+1] = rgb888[3*i+1];
		bgra8888[4*i+2] = rgb888[3*i];
		bgra8888[4*i+3] = alpha;
	}
}

void rgb565_to_rgb888(uint16_t* rgb565, uint8_t* rgb888, const int pixNum) {
	for(int i = 0; i < pixNum; ++i) {
		rgb888[3*i]   = ((rgb565[i] & 0b1111100000000000) >> 11) * 255 / 31;
		rgb888[3*i+1] = ((rgb565[i] & 0b0000011111100000) >> 5)  * 255 / 63;
		rgb888[3*i+2] =  (rgb565[i] & 0b0000000000011111)        * 255 / 31;
	}
}

void test1() {
	const int frameBufPixN = ScreenWidth * ScreenHeight;
	const int spritePixN = SpriteSize * SpriteSize;

	uint16_t frameBuffer[frameBufPixN];
	uint8_t outBuffer[3*frameBufPixN];
	uint8_t sprite[4 * spritePixN];

	FILE* fd;
	uint8_t tmpBuf[frameBufPixN];


	fd = fopen("buffer.ppm", "r");
	ppm_to_rgb888(fd, tmpBuf, ScreenWidth, ScreenHeight);
	fclose(fd);
	rgb888_to_rgb565(tmpBuf, frameBuffer, frameBufPixN);

	fd = fopen("sprite.ppm", "r");
	ppm_to_rgb888(fd, tmpBuf, SpriteSize, SpriteSize);
	fclose(fd);

	rgb888_to_bgra8888(tmpBuf, sprite, spritePixN, 255);
	blitSprite(frameBuffer, sprite, -32, -32);

	blitSprite(frameBuffer, sprite, ScreenWidth-32, ScreenHeight-32);

	float alpha = 0;
	for(int i = 0; i <= 1024; i += 64){
		rgb888_to_bgra8888(tmpBuf, sprite, spritePixN, (uint8_t)(alpha));
		blitSprite(frameBuffer, sprite, i, 100);
		alpha += 7.96875;
	}
	for(int i = 0; i <= 1024; i += 64){
		rgb888_to_bgra8888(tmpBuf, sprite, spritePixN, (uint8_t)(alpha));
		blitSprite(frameBuffer, sprite, i, 164);
		alpha += 7.96875;
	}

	rgb565_to_rgb888(frameBuffer, outBuffer, frameBufPixN);
	fd = fopen("out.ppm", "w");
	rgb888_to_ppm(fd, outBuffer, ScreenWidth, ScreenHeight);
	fclose(fd);
}

struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    long diffInNanos = end_time.tv_nsec - start_time.tv_nsec;
    return diffInNanos;
}

void testRndPos(int seed, int nSprites, int iterNum) {

	const int frameBufPixN = ScreenWidth * ScreenHeight;
	const int spritePixN = SpriteSize * SpriteSize;

	uint16_t frameBuffer[frameBufPixN];
	uint8_t outBuffer[3*frameBufPixN];
	uint8_t sprite[4 * spritePixN];

	FILE* fd;
	uint8_t tmpBuf[frameBufPixN];

	int rndXCoords[nSprites];
	int rndYCoords[nSprites];

	srand(seed);

	fd = fopen("buffer.ppm", "r");
	ppm_to_rgb888(fd, tmpBuf, ScreenWidth, ScreenHeight);
	fclose(fd);
	rgb888_to_rgb565(tmpBuf, frameBuffer, frameBufPixN);

	fd = fopen("sprite.ppm", "r");
	ppm_to_rgb888(fd, tmpBuf, SpriteSize, SpriteSize);
	fclose(fd);

	rgb888_to_bgra8888(tmpBuf, sprite, spritePixN, 127);

	buildLUTs();

    for(int i = 0; i < nSprites; ++i) {
	    rndXCoords[i] = (rand() % (ScreenWidth  + 2*SpriteSize )) - SpriteSize;
	    rndYCoords[i] = (rand() % (ScreenHeight + 2*SpriteSize )) - SpriteSize;
    }

    unsigned long long avgSum = 0;
    long maxVal, minVal;
    for(int i = 0; i < iterNum; ++i) {
	    struct timespec startTime;
	    long deltaTime;

	    startTime = timer_start();
	    for(int j = 0; j < nSprites; ++j) {
		    blitSprite(frameBuffer, sprite, rndXCoords[j], rndYCoords[j]);
	    }
	    deltaTime = timer_end(startTime);

	    if(i == 0) {maxVal = minVal = deltaTime;}
	    if(deltaTime < minVal) {minVal = deltaTime;}
	    if(deltaTime > maxVal) {maxVal = deltaTime;}
	    avgSum += deltaTime;
    }
    printf("Average on %d iterations: %u ns\n", iterNum, (avgSum - minVal - maxVal) / (iterNum-2));
	

	rgb565_to_rgb888(frameBuffer, outBuffer, frameBufPixN);
	fd = fopen("out.ppm", "w");
	rgb888_to_ppm(fd, outBuffer, ScreenWidth, ScreenHeight);
	fclose(fd);
}

int main() {
	testRndPos(1923, 20, 20000);
}
