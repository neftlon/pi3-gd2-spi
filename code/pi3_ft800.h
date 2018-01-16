#if !defined(PI3_FT800_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2018. All Rights Reserved. $
   ======================================================================== */
/*
  NOTE(js): Most operations/names/values are directly borrowed from
  the official FT800 documentation an programmers guide (1 and 2).
   (1) <http://brtchip.com/wp-content/uploads/Support/Documentation/
        Datasheets/ICs/EVE/DS_FT800_Embedded_Video_Engine.pdf>
   (2) <http://brtchip.com/wp-content/uploads/Support/Documentation/
        Programming_Guides/ICs/EVE/FT800_Series_Programmer_Guide.pdf>
 */

/* 
  TODO(js) -- Common to do:
    - This library must not be (?) specific for the FT800 within the
      Gameduino 2 Arduino shield, must it?
    - Maybe un-hardcode the commands (why would anyone need this..?)
    - Functions to implement (?):
    
#define BITMAP_SOURCE(addr) ((1UL<<24)|(((addr)&1048575UL)<<0))
#define CLEAR_COLOR_RGB(red,green,blue) ((2UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define TAG(s) ((3UL<<24)|(((s)&255UL)<<0))
#define BITMAP_HANDLE(handle) ((5UL<<24)|(((handle)&31UL)<<0))
#define CELL(cell) ((6UL<<24)|(((cell)&127UL)<<0))
#define BITMAP_LAYOUT(format,linestride,height) ((7UL<<24)|(((format)&31UL)<<19)|(((linestride)&1023UL)<<9)|(((height)&511UL)<<0))
#define BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((8UL<<24)|(((filter)&1UL)<<20)|(((wrapx)&1UL)<<19)|(((wrapy)&1UL)<<18)|(((width)&511UL)<<9)|(((height)&511UL)<<0))
#define ALPHA_FUNC(func,ref) ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0))
#define STENCIL_FUNC(func,ref,mask) ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0))
#define BLEND_FUNC(src,dst) ((11UL<<24)|(((src)&7UL)<<3)|(((dst)&7UL)<<0))
#define STENCIL_OP(sfail,spass) ((12UL<<24)|(((sfail)&7UL)<<3)|(((spass)&7UL)<<0))
#define LINE_WIDTH(width) ((14UL<<24)|(((width)&4095UL)<<0))
#define CLEAR_COLOR_A(alpha) ((15UL<<24)|(((alpha)&255UL)<<0))
#define COLOR_A(alpha) ((16UL<<24)|(((alpha)&255UL)<<0))
#define CLEAR_STENCIL(s) ((17UL<<24)|(((s)&255UL)<<0))
#define CLEAR_TAG(s) ((18UL<<24)|(((s)&255UL)<<0))
#define STENCIL_MASK(mask) ((19UL<<24)|(((mask)&255UL)<<0))
#define TAG_MASK(mask) ((20UL<<24)|(((mask)&1UL)<<0))
#define BITMAP_TRANSFORM_A(a) ((21UL<<24)|(((a)&131071UL)<<0))
#define BITMAP_TRANSFORM_B(b) ((22UL<<24)|(((b)&131071UL)<<0))
#define BITMAP_TRANSFORM_C(c) ((23UL<<24)|(((c)&16777215UL)<<0))
#define BITMAP_TRANSFORM_D(d) ((24UL<<24)|(((d)&131071UL)<<0))
#define BITMAP_TRANSFORM_E(e) ((25UL<<24)|(((e)&131071UL)<<0))
#define BITMAP_TRANSFORM_F(f) ((26UL<<24)|(((f)&16777215UL)<<0))
#define SCISSOR_XY(x,y) ((27UL<<24)|(((x)&511UL)<<9)|(((y)&511UL)<<0))
#define SCISSOR_SIZE(width,height) ((28UL<<24)|(((width)&1023UL)<<10)|(((height)&1023UL)<<0))
#define CALL(dest) ((29UL<<24)|(((dest)&65535UL)<<0))
#define JUMP(dest) ((30UL<<24)|(((dest)&65535UL)<<0))
#define COLOR_MASK(r,g,b,a) ((32UL<<24)|(((r)&1UL)<<3)|(((g)&1UL)<<2)|(((b)&1UL)<<1)|(((a)&1UL)<<0))
#define CLEAR(c,s,t) ((38UL<<24)|(((c)&1UL)<<2)|(((s)&1UL)<<1)|(((t)&1UL)<<0))
#define SAVE_CONTEXT() ((34UL<<24))
#define RESTORE_CONTEXT() ((35UL<<24))
#define RETURN() ((36UL<<24))
#define MACRO(m) ((37UL<<24)|(((m)&1UL)<<0))
 */

/* NOTE(js): Primitive drawing types
 * (Can be passed into BeginPrimitive). */
#define PRIMITIVE_TYPE_BITMAPS 1 // Bitmap drawing primitive
#define PRIMITIVE_TYPE_POINTS 2 // Point drawing primitive
#define PRIMITIVE_TYPE_LINES 3 // Line drawing primitive
#define PRIMITIVE_TYPE_LINE_STRIP 4 // Line strip drawing primitive
#define PRIMITIVE_TYPE_EDGE_STRIP_R 5 //Edge strip right side drawing primitive
#define PRIMITIVE_TYPEEDGE_STRIP_L 6 // Edge strip left side drawing primitive
#define PRIMITIVE_TYPE_EDGE_STRIP_A 7 // Edge strip above drawing primitive
#define PRIMITIVE_TYPE_EDGE_STRIP_B 8 // Edge strip below side drawing primitive
#define PRIMITIVE_TYPE_RECTS 9 // Rectangle drawing primitive

// NOTE(js): Clear command bits
#define COLOR_BUFFER_BIT   0x1
#define STENCIL_BUFFER_BIT 0x2
#define TAG_BUFFER_BIT     0x4
#define CLEAR_ALL_BUFFERS  0x7

struct ft800
{
    // NOTE(js): File descriptor/handle of the SPI-connection
    int FileDesc;
    // NOTE(js): SPI channel
    int Channel;
    int unsigned DisplayListPointerOffset;
};

inline int
GetDisplayListUsedMemory(ft800 *Device)
{
    int Result = -1;
    if(Device && Device->FileDesc)
    {
        Result = Device->DisplayListPointerOffset / 8;
    }
    return(Result);
}

extern "C"
{
    int Prepare(ft800 *Device,
                int SPIChannel, int SPIConnectionSpeed);

    void PushClear(ft800 *Device, char unsigned ClearMask,
                   float R, float G, float B, float A = 1.0f);

    void PushVertex2f(ft800 *Device, float x, float y);

    void PushVertex2i(ft800 *Device,
                      int x, int y,
                      int Handle, int Cell);

    void BeginDisplayList(ft800 *Device);
    
    void SetColorRGB(ft800 *Device,
                     char unsigned R,
                     char unsigned G,
                     char unsigned B);
        
    void SetPointSize(ft800 *Device, short int unsigned Size);

    void BeginPrimitive(ft800 *Device, int PrimitiveType);

    void EndPrimitive(ft800 *Device);

    /* NOTE(js): Execute/Run the commands stored in the display list
     * locally on the FT800 chip. */
    void ExecuteDisplayList(ft800 *Device);
};

#define PI3_FT800_H
#endif
