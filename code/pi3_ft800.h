#if !defined(PI3_FT800_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2018. All Rights Reserved. $
   ======================================================================== */
/* 
  TODO(js) -- Common to do:
    - This library must not be (?) specific for the FT800 within the
      Gameduino 2 Arduino shield, must it?
    - Maybe un-hardcode the commands (why would anyone need this..?)
 */

struct ft800
{
    // NOTE(js): File descriptor/handle of the SPI-connection
    int FileDesc;
    // NOTE(js): SPI channel
    int Channel;
    int unsigned DisplayListPointerOffset;
};

extern "C"
{
    int Prepare(ft800 *Device,
                int SPIChannel, int SPIConnectionSpeed);

    void PushClear(ft800 *Device,
                   float R, float G, float B, float A = 1.0f);

    void PushVertex2f(ft800 *Device, float x, float y);

    void PushVertex2i(ft800 *Device,
                      int x, int y,
                      int Handle, int Cell);

    void BeginPrimitive(ft800 *Device, int PrimitiveType);

    void EndPrimitive(ft800 *Device);

    /* NOTE(js): Execute/Run the commands stored in the display list
     * locally on the FT800 chip. */
    void ExecuteDisplayList(ft800 *Device);
};

#define PI3_FT800_H
#endif
