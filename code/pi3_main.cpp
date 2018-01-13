/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2017. All Rights Reserved. $
   ======================================================================== */
#include "pi3_ft800.cpp"

int main(int ArgCount, char **Args)
{    
    int Channel = 0;
    int Speed = 500000;
    
    ft800 Handle = OpenFT800(Channel, Speed);
    if(IsValid(&Handle))
    {
        HostCommand(&Handle, 0x62); // CLK_48M Intern
        HostCommand(&Handle, 0x00); // ACTIVE
        usleep(20 * 1000);
        
        char unsigned ChipID = 0;
        do
        {
            ChipID = Read8(&Handle, 0x102400);
            printf("Checking ChipID... Is 0x%x.\n", ChipID);
        } while(ChipID != 0x7C);
        printf("Done checking ChipID.\n");

        int unsigned ROMChipIDRegister = Read32(&Handle, ROM_CHIPID);
        short int ROMChipID = (ROMChipIDRegister >> 16) & 0xFFFF;
        short int VersionID = ROMChipIDRegister & 0xFFFF;
        printf("ROMChipID: %04x, VersionID: %04x\n",
               (int)ROMChipID, (int)VersionID);
        
        Write16(&Handle, REG_HCYCLE, 548);
        Write16(&Handle, REG_HOFFSET, 43);
        Write16(&Handle, REG_HSYNC0, 0);
        Write16(&Handle, REG_HSYNC1, 41);
        
        Write16(&Handle, REG_VCYCLE, 292);
        Write16(&Handle, REG_VOFFSET, 12);
        Write16(&Handle, REG_VSYNC0, 0);
        Write16(&Handle, REG_VSYNC1, 10);
        
        Write8(&Handle, REG_SWIZZLE, 0);
        Write8(&Handle, REG_PCLK_POL, 1);
        
        Write8(&Handle, REG_CSPREAD, 1);
        Write16(&Handle, REG_HSIZE, 480);
        Write16(&Handle, REG_VSIZE, 272);

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
               (float)Handle.DisplayListPointerOffset/8.0f);

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
