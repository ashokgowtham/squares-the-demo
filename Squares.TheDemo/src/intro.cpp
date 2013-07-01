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

bool BlackToWhiteFade(unsigned int *buffer, unsigned long time)
{
    unsigned int col;
    unsigned int r = (time*0xaa/1000)&0xff;
    unsigned int g = (time*0xcc/1000)&0xff;
    unsigned int b = (time*0xff/1000)&0xff;
    col = 0xff000000 | r<<16 | g<<8 | b;
    for( int i=0; i<XRES*YRES; i++ )
    {
        *buffer++ = col;
    }
    if (time<1000)
    {
        return false;
    }
    return true;
}

bool initro(unsigned int *buffer, unsigned long time)
{
    unsigned int col;
    unsigned int r = (time*255/2500)&0xff;
    unsigned int g = (time*255/500)&0xff;
    unsigned int b = (time*255/5000)&0xff;
    col = 0xff000000 | r<<16 | g<<8 | b;
    col = 0xffaaccff;
    for( int i=0; i<XRES*YRES; i++ )
    {
        *buffer++ = col;
    }
    if (time<5000)
    {
        return false;
    }
    return true;
}

bool WhiteToBlackFade(unsigned int *buffer, unsigned long time)
{
    unsigned int col;
    col = (255-time*255/1000)&0xff;
    col = 0xff000000 | col<<16 | col<<8 | col;
    for( int i=0; i<XRES*YRES; i++ )
    {
        *buffer++ = col;
    }
    if (time<1000)
    {
        return false;
    }
    return true;
}

CLIP clips[] = {
    BlackToWhiteFade,
    initro,
    WhiteToBlackFade,
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
