/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2017. All Rights Reserved. $
   ======================================================================== */
//#define ArrayCount(Array) (sizeof(Array)/sizeof(Array[0]))

#include "pi3_ft800.h"

#include <stdio.h>
#include <unistd.h>

static void
JohannesDemo(ft800 *Device, float dt)
{ 
    static short int unsigned xOffset = 0;
    xOffset += (int)(dt*100.0f);
    
    PushClear(Device, CLEAR_ALL_BUFFERS, 1.0f, 0.0f, 0.0f);
    BeginPrimitive(Device, 1);
    int x = xOffset + 10;
    int CharacterOffset = 0;
    PushVertex2i(Device, x + CharacterOffset, 110, 31, 'J');
    CharacterOffset += 23;
    PushVertex2i(Device, x + CharacterOffset, 110, 31, 'o');
    CharacterOffset += 24;
    PushVertex2i(Device, x + CharacterOffset, 110, 31, 'h');
    CharacterOffset += 24;
    PushVertex2i(Device, x + CharacterOffset, 110, 31, 'a');
    CharacterOffset += 23;
    PushVertex2i(Device, x + CharacterOffset, 110, 31, 'n');
    CharacterOffset += 24;
    PushVertex2i(Device, x + CharacterOffset, 110, 31, 'n');
    CharacterOffset += 24;
    PushVertex2i(Device, x + CharacterOffset, 110, 31, 'e');
    CharacterOffset += 22;
    PushVertex2i(Device, x + CharacterOffset, 110, 31, 's');
    EndPrimitive(Device);

    SetColorRGB(Device, 0xA0, 0x16, 0x16);

    BeginPrimitive(Device, PRIMITIVE_TYPE_RECTS);
    PushVertex2f(Device, 10.0f * 16.0f, 10.0f * 16.0f);
    PushVertex2f(Device, 100.0f * 16.0f, 100.0f * 16.0f);
    PushVertex2f(Device, 150.0f * 16.0f, 10.0f * 16.0f);
    PushVertex2f(Device, 250.0f * 16.0f, 100.0f * 16.0f);
    EndPrimitive(Device);
}

static void
FTDIDemo(ft800 *Device, float dt)
{    
    static short int unsigned xOffset = 0;
    xOffset += (int)(dt*100.0f);
    
    PushClear(Device, CLEAR_ALL_BUFFERS, 0.0f, 0.0f, 0.0f);
    BeginPrimitive(Device, PRIMITIVE_TYPE_BITMAPS);
    PushVertex2i(Device, xOffset + 220, 110, 31, 'F'); // ascii F in font 31
    PushVertex2i(Device, xOffset + 244, 110, 31, 'T'); // ascii T
    PushVertex2i(Device, xOffset + 270, 110, 31, 'D'); // ascii D
    PushVertex2i(Device, xOffset + 299, 110, 31, 'I'); // ascii I
    EndPrimitive(Device);
    
    SetColorRGB(Device, 160, 22, 22); // change color to red
    SetPointSize(Device, 320);  // set point size to 20 pixels in radius

    BeginPrimitive(Device, PRIMITIVE_TYPE_POINTS);
    PushVertex2i(Device, xOffset + 192, 133, 0, 0); // red point
    EndPrimitive(Device);

    if(xOffset >= 492)
    {
        xOffset = 0;
    }
}

int main(int ArgCount, char **Args)
{    
    int Channel = 0;
    int Speed = 500000;
    ft800 Handle;
    if(Prepare(&Handle, Channel, Speed))    
    {
        //JohannesDemo(&Handle);
        printf("%.02fB of graphics ROM used.\n",
               (float)GetDisplayListUsedMemory(&Handle));

        float MillisecondDelay = 20;
        float dt = (float)MillisecondDelay / 1000.0f;
        for(;;)
        {
            JohannesDemo(&Handle, dt);
        
            ExecuteDisplayList(&Handle);
            usleep(MillisecondDelay * 1000);        
        }
    }
    else
    {
        fprintf(stderr, "Failed to get FileDesc.\n");
    }
    
    return(0);
}
