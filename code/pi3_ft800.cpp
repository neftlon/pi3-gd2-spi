/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Johannes Spies $
   $Notice: (C) Copyright 2017. All Rights Reserved. $
   ======================================================================== */
#include "pi3_ft800.h"

// TODO(js): Use open or something instead?
#include <wiringPiSPI.h>

#include <unistd.h>

#define MOSI 10
#define MISO 9
#define SCK 11

#define CLKEXT       0x44

#define REG_ID       0x102400
#define REG_HCYCLE   0x102428
#define REG_HOFFSET  0x10242C
#define REG_HSIZE    0x102430
#define REG_HSYNC0   0x102434 
#define REG_HSYNC1   0x102438 
#define REG_VCYCLE   0x10243C
#define REG_VOFFSET  0x102440
#define REG_VSIZE    0x102444 
#define REG_VSYNC0   0x102448
#define REG_VSYNC1   0x10244C
#define REG_SWIZZLE  0x102460
#define REG_CSPREAD  0x102464 
#define REG_PCLK_POL 0x102468
#define REG_DLSWAP   0x102450
#define REG_GPIO_DIR 0x10248C
#define REG_GPIO     0x102490
#define REG_PCLK     0x10246C
#define REG_VOL_SOUND 0x102480
#define REG_SOUND    0x102484
#define REG_PLAY     0x102488

#define PLL_FREQUENCY_48MHz 0x62
#define PLL_FREQUENCY_36MHz 0x61
#define PLL_FREQUENCY_24MHz 0x64

#define RST 0x68

#define ROM_CHIPID 0x0C0000

#define RAM_DL 0x100000

#define ArrayCount(Array) (sizeof(Array)/sizeof(Array[0]))

static int
SPIOpenDevice(int Channel, int Speed)
{
    wiringPiSPISetup(Channel, Speed);
}

static void
SPIReadWrite(ft800 *Device, char unsigned *Buffer, int Length)
{
    wiringPiSPIDataRW(Device->Channel, Buffer, Length);
}

static void
HostMemoryWrite(ft800 *Device, int unsigned Address,
                int unsigned DataBufferLength,
                char unsigned *DataBuffer)
{
    // NOTE(js): 4 byte header
    int DataOffset = 3;
    int TotalBufferSize = DataBufferLength + DataOffset;
    char unsigned Buffer[TotalBufferSize];
    Buffer[0] = 0x80 | (Address >> 16) & 0x3F;
    Buffer[1] = (Address >> 8) & 0xFF;
    Buffer[2] = Address & 0xFF;
        
    char unsigned *BufferPtr = Buffer + DataOffset;
    for(int ItemIndex = 0;
        ItemIndex < DataBufferLength;
        ++ItemIndex)
    {
        *BufferPtr++ = *DataBuffer++;
    }
    
    SPIReadWrite(Device, Buffer, TotalBufferSize);
}

static void
HostMemoryRead(ft800 *Device, int unsigned Address,
               char unsigned *DataBuffer,
               int unsigned DataBufferLength)
{
    // NOTE(js): 4 byte header
    int DataOffset = 3 + 1;
    int TotalBufferSize = DataBufferLength + DataOffset;
    char unsigned Buffer[TotalBufferSize];
    Buffer[0] = (Address >> 16) & 0x3F;
    Buffer[1] = (Address >> 8) & 0xFF;
    Buffer[2] = Address & 0xFF;
    Buffer[3] = 0x00; // NOTE(js): Dummy byte

    SPIReadWrite(Device, Buffer, TotalBufferSize);

    for(int Index = 0;
        Index < DataBufferLength;
        ++Index)
    {
        DataBuffer[Index] = Buffer[DataOffset + Index];
    }
}

static void
HostCommand(ft800 *Device, char unsigned Command)
{
    /* NOTE(js): According to the FT800 documentation,
     * a host sends a command by writing three bytes,
     * the first has the two first bits set to 0 1 and
     * the following 6 bits are the command. The
     * following two bytes are 0x0. */
    char unsigned Buffer[3];
    Buffer[0] = Command;
    Buffer[1] = 0x0;
    Buffer[2] = 0x0;
    SPIReadWrite(Device, Buffer, ArrayCount(Buffer));
}

static void
Write8(ft800 *Device, int unsigned Address, char unsigned Value)
{
    HostMemoryWrite(Device, Address, 1, &Value);
}

static void
Write16(ft800 *Device, int unsigned Address,
        short int unsigned Value)
{
    HostMemoryWrite(Device, Address, 2, (char unsigned *)&Value);
}

static void
Write32(ft800 *Device, int unsigned Address,
        int unsigned Value)
{
    HostMemoryWrite(Device, Address, 4, (char unsigned *)&Value);
}

static char unsigned
Read8(ft800 *Device, int unsigned Address)
{
    char unsigned Result = 0;
    HostMemoryRead(Device, Address, &Result, 1);
    return(Result);
}

static short int unsigned
Read16(ft800 *Device, int unsigned Address)
{
    short int unsigned Result = 0;
    HostMemoryRead(Device, Address, (char unsigned *)&Result, 2);
    return(Result);
}

static int unsigned
Read32(ft800 *Device, int unsigned Address)
{
    int unsigned Result = 0;
    HostMemoryRead(Device, Address, (char unsigned *)&Result, 4);
    return(Result);
}

static void
EstablishSPIConnection(ft800 *Device, int Channel, int Speed)
{
    Device->FileDesc = SPIOpenDevice(Channel, Speed);
    Device->Channel = Channel;
    Device->DisplayListPointerOffset = 0;
}

// TODO(js): Some error handling...?
static void
InitDevice(ft800 *Device)
{
    HostCommand(Device, 0x62); // CLK_48M Intern
    HostCommand(Device, 0x00); // ACTIVE
    // TODO(js): Is this neccessary?
    usleep(20 * 1000);

    char unsigned ChipID = 0;
    do
    {
        ChipID = Read8(Device, 0x102400);
        //printf("Checking ChipID... Is 0x%x.\n", ChipID);
    } while(ChipID != 0x7C);
    //printf("Done checking ChipID.\n");

    int unsigned ROMChipIDRegister = Read32(Device, ROM_CHIPID);
    short int ROMChipID = (ROMChipIDRegister >> 16) & 0xFFFF;
    short int VersionID = ROMChipIDRegister & 0xFFFF;
    //printf("ROMChipID: %04x, VersionID: %04x\n",
    //       (int)ROMChipID, (int)VersionID);
        
    Write16(Device, REG_HCYCLE, 548);
    Write16(Device, REG_HOFFSET, 43);
    Write16(Device, REG_HSYNC0, 0);
    Write16(Device, REG_HSYNC1, 41);
        
    Write16(Device, REG_VCYCLE, 292);
    Write16(Device, REG_VOFFSET, 12);
    Write16(Device, REG_VSYNC0, 0);
    Write16(Device, REG_VSYNC1, 10);
        
    Write8(Device, REG_SWIZZLE, 0);
    Write8(Device, REG_PCLK_POL, 1);
        
    Write8(Device, REG_CSPREAD, 1);
    Write16(Device, REG_HSIZE, 480);
    Write16(Device, REG_VSIZE, 272);       
}

int Prepare(ft800 *Device, int SPIChannel, int SPIConnectionSpeed)
{
    int Result = 1;
    if(Device)
    {
        EstablishSPIConnection(Device,
                               SPIChannel, SPIConnectionSpeed);
        if(Device->FileDesc != -1)
        {
            InitDevice(Device);
            Result = 0; // NOTE(js): Operation/Initialisation failed
        }
        else
        {
            // TODO(js): Logging -> Failed to open SPI connection
        }
    }
    else
    {
        // TODO(js): Logging -> Invalid input
    }
    return(Result);
}

static void
PushDisplayListCommand(ft800 *Device, int unsigned Command)
{
    Write32(Device, RAM_DL + Device->DisplayListPointerOffset,
            Command);
    Device->DisplayListPointerOffset += 4;
}

void ExecuteDisplayList(ft800 *Device)
{    
    PushDisplayListCommand(Device, 0x00000000);

    Write8(Device, REG_DLSWAP, 2);
        
    char unsigned GPIODirRegisterValue = Read8(Device, REG_GPIO_DIR);
    Write8(Device, REG_GPIO_DIR, 0x80 | GPIODirRegisterValue);
    char unsigned GPIORegisterValue = Read8(Device, REG_GPIO);
    Write8(Device, REG_GPIO, 0x80 | GPIORegisterValue);
        
    Write8(Device, REG_PCLK, 5);

    Device->DisplayListPointerOffset = 0;
}

void PushClear(ft800 *Device, float R, float G, float B, float A)
{
    char RByte = (char)(R * 255.0f);
    char GByte = (char)(G * 255.0f);
    char BByte = (char)(B * 255.0f);
       
    // NOTE(js): Memory order seems to be BB GG RR    
    int RGBColorCode = ((RByte | (GByte << 8) | (BByte << 16)) & 0x00FFFFFF);
    int SetClearColorRGBCommand = 0x02000000;
    PushDisplayListCommand(Device,
                           SetClearColorRGBCommand | RGBColorCode);

    int AColorCode = ((int)(A * 255.0f) & 0xFF);
    int SetClearColorACommand = 0x0F000000;
    PushDisplayListCommand(Device,
                           SetClearColorACommand | AColorCode);

    // NOTE(js): Clear command
    PushDisplayListCommand(Device, 0x26000004);
}

void PushVertex2f(ft800 *Device, float x, float y)
{
    short int ClampedX = (short int)x & 0x7FFF;
    short int ClampedY = (short int)y & 0x7FFF;

    int Vertex2fCommand = (0x1 << 30);
    PushDisplayListCommand(Device, Vertex2fCommand | (ClampedX << 15) | ClampedY);
}

void PushVertex2i(ft800 *Device, int x, int y, int Handle, int Cell)
{
    int Vertex2iCommand = (0x2 << 30);
    short int ClampedX = (short int)x & 0x1FF;
    short int ClampedY = (short int)y & 0x1FF;
    char unsigned ClampedHandle = (char)Handle & 0x1F;
    char unsigned ClampedCell = (char)Cell & 0x7F;
    PushDisplayListCommand(Device, (Vertex2iCommand |
                                    (ClampedX << 21) |
                                    (ClampedY << 12) |
                                    (ClampedHandle << 7) |
                                    ClampedCell));
}

void BeginPrimitive(ft800 *Device, int PrimitiveType)
{
    char unsigned ClampedPrimitiveType = (char)(PrimitiveType & 0xF);
    int unsigned BeginCommand = 0x1F000000;
    PushDisplayListCommand(Device, (BeginCommand |
                                    ClampedPrimitiveType));
}

void EndPrimitive(ft800 *Device)
{
    PushDisplayListCommand(Device, 0x21000000);
}
