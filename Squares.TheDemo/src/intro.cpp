/*/                                                                        \*\
|*|         ____      ________________________________  ______   ____      |*|
|*|        /    \    /  ____________________________  \ \__   | /  _/      |*|
|*|       /  /\  \   | |       |  |    |  |   /  /  \  \   |  |/  /        |*|
|*|      /  /  \  \  | |_____  |  |____|  |  |  |    |  |  |  |  /         |*|
|*|     /  /____\  \ \_____  \ |   ____   |  |  |    |  |  |  | /\         |*|
|*|    /  ________  \      | | |  |    |  |  |  |    |  |  |  |/  \        |*|
|*|   /  /   _____\__\_____| | |  |    |  |   \  \__/  /   |  | \  \_      |*|
|*|  /__/   /________________/ |__|    |__|    \______/    |__|  \___\     |*|
|*|                                                                        |*|
|*|                                         Codes by, Ashok Gowtham M.     |*|
|*|                                           2013 June                    |*|
\*\                                                                        /*/

#include "intro.h"

unsigned long starttime;
int clipIndex;
int clipCount;
typedef bool (*CLIP)(unsigned int *buffer, unsigned long time);

#define RGB(R,G,B) (0xff000000|((R&0xff)<<16)|((G&0xff)<<8)|(B&0xff))

#define GRAY(GrayLevel) RGB(GrayLevel,GrayLevel,GrayLevel)

bool __inline Progress(unsigned int time, unsigned int duration)
{
	return !(time<duration);
}

void DrawSolidColor(unsigned int *buffer, unsigned int color)
{
	for( int i=0; i<XRES*YRES; i++ )
	{
		*buffer++ = color;
	}
}

unsigned __inline int lerp(unsigned int fromColor, unsigned int toColor, unsigned int a, unsigned b) //same as lerp(from,to,a/b); just a perf opt.
{
	unsigned int b1 = (fromColor    ) & 0xff;
	unsigned int g1 = (fromColor>>8 ) & 0xff;
	unsigned int r1 = (fromColor>>16) & 0xff;
	unsigned int a1 = (fromColor>>24) & 0xff;

	unsigned int b2 = (fromColor    ) & 0xff;
	unsigned int g2 = (fromColor>>8 ) & 0xff;
	unsigned int r2 = (fromColor>>16) & 0xff;
	unsigned int a2 = (fromColor>>24) & 0xff;
	
	unsigned int B = b1+(b2-b1)*a/b;
	unsigned int G = g1+(g2-g1)*a/b;
	unsigned int R = r1+(r2-r1)*a/b;
	unsigned int A = a1+(a2-a1)*a/b;
	
	return A<<24 | R<<16 | G<<8 | B;
}

void DrawFade(unsigned int *buffer, unsigned int time, unsigned int duration, unsigned int fromColor, unsigned int toColor)
{
	unsigned int col = lerp(fromColor,toColor,time,duration);
	DrawSolidColor(buffer,col);
}

void DrawWhiteToBlackFade(unsigned int *buffer, unsigned int time)
{
	unsigned int col;
	col = (255-time*255/1000)&0xff;
	col = 0xff000000 | col<<16 | col<<8 | col;
	unsigned int FromCol=0xff000000;
	unsigned int ToCol=0xffffffff;
	DrawFade(buffer,time,1000,FromCol,ToCol);
}

void DrawBlackToWhiteFade(unsigned int *buffer, unsigned int time)
{
	unsigned int col;
	unsigned int r = (time*0xaa/1000)&0xff;
	unsigned int g = (time*0xcc/1000)&0xff;
	unsigned int b = (time*0xff/1000)&0xff;
	col = 0xff000000 | r<<16 | g<<8 | b;
	DrawSolidColor(buffer,col);
}

void DrawSquaredPattern(unsigned int *buffer, unsigned long time)
{
	unsigned int col;
	for( int i=0; i<XRES*YRES; i++ )
	{
		int x = i%XRES;
		int y = i/XRES;
		*buffer++ = GRAY((i^y));
	}
}

bool ShowBlackToWhiteFade(unsigned int *buffer, unsigned long time)
{
	DrawBlackToWhiteFade(buffer,time);
	return Progress(time,1000);
}

bool ShowLogo(unsigned int *buffer, unsigned long time)
{
	DrawSquaredPattern(buffer,time);
	return Progress(time,2000);
}

bool initro(unsigned int *buffer, unsigned long time)
{
	DrawSolidColor(buffer,0xffaaccff);
	return Progress(time,5000);
}

bool ShowWhiteToBlackFade(unsigned int *buffer, unsigned long time)
{
	DrawWhiteToBlackFade(buffer,time);
	return Progress(time,1000);
}

CLIP clips[] = {
	ShowBlackToWhiteFade,
	ShowLogo,
	initro,
	ShowWhiteToBlackFade,
};

bool currentShowClip( unsigned int *buffer, unsigned long time )
{
	return clips[clipIndex](buffer,time);
}

bool advanceClip( void )
{
	if ( ++clipIndex < clipCount )
	{
		return false;
	}
	return true;
}

bool doShow( unsigned int *buffer, unsigned long time )
{
	bool done = currentShowClip(buffer, time-starttime);
	if (done)
	{
		starttime = time;
		return advanceClip();
	}
	return false;
}

void initClips( void )
{
	clipIndex=0;
	clipCount=sizeof(clips)/sizeof(CLIP);
}

void intro_init( unsigned long startTime )
{
	starttime = startTime;
	initClips();
}

bool intro_do( unsigned int *buffer, unsigned long time )
{
	return doShow(buffer,time);
}
