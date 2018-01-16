/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2017. All Rights Reserved. $
   ======================================================================== */
//#define ArrayCount(Array) (sizeof(Array)/sizeof(Array[0]))

#include <stdio.h>

#include "pi3_ft800.h"

int main(int ArgCount, char **Args)
{    
    int Channel = 0;
    int Speed = 500000;
    ft800 Handle;
    if(Prepare(&Handle, Channel, Speed))    
    {        
        PushClear(&Handle, 0.0f, 1.0f, 0.0f);
        BeginPrimitive(&Handle, 1);
        int x = 10;
        int CharacterOffset = 0;
        PushVertex2i(&Handle, x + CharacterOffset, 110, 31, 'J');
        CharacterOffset += 23;
        PushVertex2i(&Handle, x + CharacterOffset, 110, 31, 'o');
        CharacterOffset += 24;
        PushVertex2i(&Handle, x + CharacterOffset, 110, 31, 'h');
        CharacterOffset += 24;
        PushVertex2i(&Handle, x + CharacterOffset, 110, 31, 'a');
        CharacterOffset += 23;
        PushVertex2i(&Handle, x + CharacterOffset, 110, 31, 'n');
        CharacterOffset += 24;
        PushVertex2i(&Handle, x + CharacterOffset, 110, 31, 'n');
        CharacterOffset += 24;
        PushVertex2i(&Handle, x + CharacterOffset, 110, 31, 'e');
        CharacterOffset += 22;
        PushVertex2i(&Handle, x + CharacterOffset, 110, 31, 's');
        EndPrimitive(&Handle);
        
        printf("%.02fB of graphics ROM used.\n",
               (float)Handle.DisplayListPointerOffset / 8.0f);

        ExecuteDisplayList(&Handle);

        for(;;)
        {
        }
    }
    else
    {
        fprintf(stderr, "Failed to get FileDesc.\n");
    }
    
    return(0);
}
