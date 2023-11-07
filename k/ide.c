#include "include/k/atapi.h"
#include "include/k/console.h"
#include "include/k/ide.h"
#include "../libs/libc/include/string.h"
#include "io.h"

// https://wiki.osdev.org/PCI_IDE_Controller

static volatile unsigned char ide_irq_invoked = 0;
IDE_Channels ide_channels[MAXIMUM_CHANNELS];
IDE_Device ide_devices[MAXIMUM_IDE_DEVICES];

static u8 IDE_read_register(u8 channel, u8 reg);
static void IDE_write_register(u8 channel, u8 reg, u8 data);
static struct SCSI_packet scsi_packet;

/**
 * Read register value from the given channel
*/
static u8 IDE_read_register(u8 channel, u8 reg)
{
    u8 result;

    // Write value ATA-control to tell IRQ is ready
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x0), 0x80 | ide_channels[channel].no_int);

    // Read register from base channel port
    if (reg < 0x08)
        result = inb(ide_channels[channel].base + reg - 0x0);
    else if (reg < 0x0C)
        result = inb(ide_channels[channel].base + reg - 0x06);
    else if (reg < 0x0E)
        result = inb(ide_channels[channel].control + reg - 0x0A);
    else if (reg < 0x16)
        result = inb(ide_channels[channel].bm_ide + reg - 0x0E);

    // Write value to tell the reading is done
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x0), ide_channels[channel].no_int);

    return result;
}

/**
 * Write data to register to the given channel
*/
static void IDE_write_register(u8 channel, u8 reg, u8 data)
{
    // Write value ATA-control to tell irq is ready
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x0), 0x80 | ide_channels[channel].no_int);

    // Write data to register ports
    if (reg < 0x08)
        outb(ide_channels[channel].base + reg - 0x0, data);
    else if (reg < 0x0C)
        outb(ide_channels[channel].base + reg - 0x06, data);
    else if (reg < 0x0E)
        outb(ide_channels[channel].control + reg - 0x0A, data); // Verify if the value is 0x0A or 0x0C
    else if (reg < 0x16)
        outb(ide_channels[channel].bm_ide + reg - 0x0E, data);

    // Write value to tell the writing is done
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x0), ide_channels[channel].no_int);
}

/** 
 * Read long word from reg port for quads times
*/
static void insl(u16 reg, u32 *buffer, int quads) {
    int index;
    for (index = 0; index < quads; index++) {
        buffer[index] = inl(reg);
    }
}

// write long word to reg port for quads times
static void outsl(u16 reg, u32 *buffer, int quads) {
    int index;
    for (index = 0; index < quads; index++) {
        outl(reg, buffer[index]);
    }
}

/**
 * Read collection of vaue from a channel into given buffer
*/
void IDE_read_buffer(u8 channel, u8 reg, u32 *buffer, u32 quads)
{
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x0), 0x80 | ide_channels[channel].no_int);

    // Get value of data-segment to extra segment by savin glast es value
    asm("pushw %es");
    asm("pushw %ax");       // Added line
    asm("movw %ds, %ax");
    asm("movw %ax, %es");
    asm("popw %ax");        // Added line

    if (reg < 0x08)
        insl(ide_channels[channel].base + reg - 0x0, buffer, quads);
    else if (reg < 0x0C)
        insl(ide_channels[channel].base + reg - 0x06, buffer, quads);
    else if (reg < 0x0E)
        insl(ide_channels[channel].control + reg - 0x0A, buffer, quads);
    else if (reg < 0x16)
        insl(ide_channels[channel].bm_ide + reg - 0x0E, buffer, quads);

    asm("popw %es");

    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x0), ide_channels[channel].no_int);
}

void IDE_write_buffer(u8 channel, u8 reg, u32 *buffer, u32 quads)
{
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x0), 0x80 | ide_channels[channel].no_int);

    // Get value of data-segment to extra segment by savin glast es value
    asm("pushw %es");
    asm("pushw %ax");       // Added line
    asm("movw %ds, %ax");
    asm("movw %ax, %es");
    asm("popw %ax");        // Added line

    if (reg < 0x08)
        outsl(ide_channels[channel].base + reg - 0x0, buffer, quads);
    else if (reg < 0x0C)
        outsl(ide_channels[channel].base + reg - 0x06, buffer, quads);
    else if (reg < 0x0E)
        outsl(ide_channels[channel].control + reg - 0x0A, buffer, quads);
    else if (reg < 0x16)
        outsl(ide_channels[channel].bm_ide + reg - 0x0E, buffer, quads);

    asm("popw %es");

    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x0), ide_channels[channel].no_int);
}

void busy_wait(u8 channel)
{
    while (IDE_read_register(channel, ATA_REG_STATUS(0x0)) & BSY)
        ; // Wait for BSY to be cleared
}

/**
 * Wait untill drive is ready, keep polling ide device until it is not busy status
*/
u8 IDE_polling(u8 channel, u8 advanced_check)
{
    // (I) Delay 400 nanoseconds for BSY to be set
    for (int i = 0; i < 4; i++)
        // Reading the Alternate Status port wastes 100ns; loop four times
        IDE_read_register(channel, ATA_REG_ALTSTATUS(0x0));

    // (II) Wait for BSY to be cleared
    busy_wait(channel);

    if (advanced_check)
    {
        // Read status register
        u8 status = IDE_read_register(channel, ATA_REG_STATUS(0x0));

        // (III) Check for errors
        if (status & ERR)
            return 2;   // Error

        // (IV) Check if device fault
        if (status & DF)
            return 1;   // Device fault
        
        // (V) Check DRQ
        // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now
        if ((status & DRQ) == 0)
            return 3;   // DRQ should be set
    }
    return 0;   // No error
}

/**
 * Print error on screen
*/
u8 IDE_print_error(u32 drive, u8 err)
{
    if (err == 0)
        return 0;   // No error to print

    console_printf("IDE:\n");
    if (err == 1)
    {
        console_printf("- Device Fault\n");
        err = 19;
    }
    else if (err == 2)
    {
        // Get error description in Error port
        u8 status = IDE_read_register(ide_devices[drive].channel, ERR);
        if (status & ATA_ER_AMNF)
        {
            console_printf("- No Address Mark Found\n");
            err = 7;
        }
        if (status & ATA_ER_TK0NF)
        {
            console_printf("- No Media or Media Error\n");
            err = 3;
        }
        if (status & ATA_ER_ABRT)
        {
            console_printf("- Command aborted\n");
            err = 20;
        }
        if (status & ATA_ER_MCR)
        {
            console_printf("- No Media or Media Error\n");
            err = 3;
        }
        if (status & ATA_ER_IDNF)
        {
            console_printf("- ID Mark Not Found\n");
            err = 21;
        }
        if (status & ATA_ER_MC)
        {
            console_printf("- No Media or Media Error\n");
            err = 3;
        }
        if (status & ATA_ER_UNC)
        {
            console_printf("- Uncorrectable Data Error\n");
            err = 22;
        }
        if (status & ATA_ER_BBK)
        {
            console_printf("- Bad Sectors\n");
            err = 13;
        }
    }
    else if (err == 3)
    {
        console_printf("- Read Nothing\n");
        err = 23;
    }
    else if (err == 4)
    {
        console_printf("- Write protected\n");
        err = 8;
    }
    console_printf("- [%s %s] %s\n",
                    (const char *[]){"Primary", "Secondary"}[ide_devices[drive].channel],   // Use the channel as an index into the array
                    (const char *[]){"Master", "Slave"}[ide_devices[drive].drive],          // Use the drive as an index into the array
                    ide_devices[drive].model);  // Drive name
    return err;
}

/**
 * 
*/
void init_IDE(u32 primary_channel_base, u32 primary_channel_control_base,
              u32 secondary_channel_base, u32 secondary_channel_control_base,
              u32 bus_master_base)
{
    int i, j, k, count = 0;
    unsigned char ide_buf[2048] = {0};

    // 1- Detect I/O Ports which interface IDE Controller
    // (Checking the address is removed for simplicty, justr assigning all ports)
    ide_channels[ATA_PORT_MASTER].base = primary_channel_base;
    ide_channels[ATA_PORT_MASTER].control = primary_channel_control_base;
    ide_channels[ATA_PORT_SLAVE].base = secondary_channel_base;
    ide_channels[ATA_PORT_SLAVE].control = secondary_channel_control_base;
    ide_channels[ATA_PORT_MASTER].base = primary_channel_base;
    ide_channels[ATA_PORT_MASTER].bm_ide = bus_master_base;
    ide_channels[ATA_PORT_SLAVE].bm_ide = bus_master_base;

    // 2- Disable IRQs in both channels by setting bit 1 (no_int) in the control port
    IDE_write_register(ATA_PORT_MASTER, ATA_REG_CONTROL(0x0), 2);
    IDE_write_register(ATA_PORT_SLAVE, ATA_REG_CONTROL(0x0), 2);

    // 3- Detect ATA/ATAPI devices
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            u8 err = 0, type = IDE_ATA, status;
            ide_devices[count].reserved = 0;    // Assuming that no drive here

            // (I) Select drive
            IDE_write_register(i, ATA_REG_DRIVE(0x0), 0xA0 | (j << 4));     // Select drive
            // sleep(1); This function still doesn't work

            // (II) Send ATA Identify Command
            IDE_write_register(i, ATA_REG_COMMAND(0x0), IDENTIFY);
            // sleep(1)

            // (III) Polling
            if (IDE_read_register(i, ATA_REG_STATUS(0x0)) == 0)
                continue;   // Is status = 0, No device
            
            while (1)
            {
                status = IDE_read_register(i, ATA_REG_STATUS(0x0));
                if (status & ERR)
                {
                    // If error, Device is not ATA
                    err = 1;
                    break;
                }
                if (!(status & BSY) && (status & DRQ))
                    break;      // Drive is not busy, and DRQ is set, everything is right
            }

            // (IV) Probe for ATAPI Devices
            if (err != 0)
            {
                unsigned char c1 = IDE_read_register(i, ATA_REG_LBA_MI(0x0));
                unsigned char ch = IDE_read_register(i, ATA_REG_LBA_HI(0x0));

                if (c1 == 0x14 && ch == 0xEB)
                    type = IDE_ATAPI;
                else if (c1 == 0x69 && ch == 0x96)
                    type = IDE_ATAPI;
                else
                    continue;       // Unknow type (may not be a device)

                IDE_write_register(i, ATA_REG_COMMAND(0x0), IDENTIFY_PACKET_DEVICE);
                // sleep(1);
            }

            // (V) Read Identification Space of the Device
            IDE_read_buffer(i, ATA_REG_DATA(0x0), (u32 *)ide_buf, 128);

            // (VI) Read Device Parameters
            ide_devices[count].reserved = 1;
            ide_devices[count].type = type;
            ide_devices[count].channel = i;
            ide_devices[count].drive = j;
            ide_devices[count].signature = *((u16 *)(ide_buf + ATA_IDENT_DEVICETYPE));
            ide_devices[count].features = *((u16 *)(ide_buf + ATA_IDENT_CAPABILITIES));
            ide_devices[count].command_sets = *((u32 *)(ide_buf + ATA_IDENT_COMMANDSETS));

            // (VII) Get size
            if (ide_devices[count].command_sets & (1 << 26))
                // Device uses 48-bit Addressing
                ide_devices[count].size = *((u32 *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
            else
                // Device uses CHS or 28-bit Addressing 
                ide_devices[count].size = *((u32 *)(ide_buf + ATA_IDENT_MAX_LBA));

            // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...)
            for (k = 0; k < 40; k += 2)
            {
                ide_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
                ide_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
            }
            ide_devices[count].model[40] = '\0'; // Terminate string
            // Remove trailing spaces in model string
            for (k = 39; k >= 0; k--)
            {
                char ch = ide_devices[count].model[k];
                if (ch == ' ')
                    ide_devices[count].model[k] = '\0';
                else
                    break;
            }

            count++;
        }
    }

    // 4- Print summary
    for (i = 0; i < 4; i++)
    {
        if (ide_devices[i].reserved == 1)
        {
            console_printf("%d:-\n", i);
            console_printf("  model: %s\n", ide_devices[i].model);
            console_printf("  type: %s\n", (const char *[]){"ATA", "ATAPI"}[ide_devices[i].type]);
            console_printf("  drive: %u, channel: %u\n", ide_devices[i].drive, ide_devices[i].channel);
            console_printf("  base: 0x%x, control: 0x%x\n", ide_channels[i].base, ide_channels[i].control);
            console_printf("  size: %u sectors, %u bytes\n", ide_devices[i].size, ide_devices[i].size * ATA_SECTOR_SIZE);
            console_printf("  signature: 0x%x, features: %d\n", ide_devices[i].signature, ide_devices[i].features);
        }
    }
}

u8 IDE_ATA_access(u8 direction, u8 drive, u32 lba, u8 nb_sects, u32 buffer)
{
    u8 lba_mode /* 0: CHS, 1: LBA28, 2: LBA48 */, dma  /* 0: No DMA, 1: DMA */, cmd;
    u8 lba_io[6];
    u8 channel = ide_devices[drive].channel;   // Read the channel
    u8 slavebit = ide_devices[drive].drive;    // Read the drive (Mater/Slave)
    u16 bus = ide_channels[channel].base;      // Bus base, like 0x1F0 which is also data port
    u32 words = 256;
    u16 cyl, i;
    u8 head, sect, err;

    IDE_write_register(channel, ATA_REG_CONTROL(0x0), ide_channels[channel]. no_int = (ide_irq_invoked = 0x0) + 0x02);

    // (I) Select one from LBA28, LBA48 or CHS
    if (lba >= 0x10000000) // Sure drive should support LBA in this case, or you are gibing a wrong LBA
    {
        // LBA48
        lba_mode = LBA_MODE_48;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0;                              // LBA28 is integer, so 32-bits are enough to access 2TB.
        lba_io[5] = 0;                              // LBA28 is integer, so 32-bits are enough to access 2TB.
        head = 0;                                   // Lower 4-bits of HDDEVSEL are not used here.
    }
    else if (ide_devices[drive].features & 0x200)   // Drive supports LBA    
    {
        // LBA28:
        lba_mode = LBA_MODE_28;
        lba_io[0] = (lba & 0x00000FF) >> 0;
        lba_io[1] = (lba & 0x000FF00) >> 8;
        lba_io[2] = (lba & 0x0FF0000) >> 16;
        lba_io[3] = 0;                              // These Registers are not used here.
        lba_io[4] = 0;                              // These Registers are not used here.
        lba_io[5] = 0;                              // These Registers are not used here.
        head = (lba & 0xF000000) >> 24;
    }
    else
    {
        // CHS:
        lba_mode = LBA_MODE_CHS;
        sect = (lba % 63) + 1;
        cyl = (lba + 1 - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = (lba + 1 - sect) % (16 * 63) / (63);  // Head number is written to ATA_REG_DRIVE lower 4-bits.
    }

    // (II) See if drive supports DMA or not
    dma = 0;    // We don't support DMA

    // (III) Wait id the drive is busy
    busy_wait(channel);

    // (IV) Select drive from the controller
    if (lba_mode == LBA_MODE_CHS)
        IDE_write_register(channel, ATA_REG_DRIVE(0x0), 0xA0 | (slavebit << 4) | head);     // Drive & CHS
    else
        IDE_write_register(channel, ATA_REG_DRIVE(0x0), 0xE0 | (slavebit << 4) | head);     // Drive & LBA
    
    // (V) Write parameters
    if (lba_mode == LBA_MODE_48)
    {
        IDE_write_register(channel, ATA_REG_SECTOR_COUNT1(0x0), 0);
        IDE_write_register(channel, ATA_REG_LBA3(0x0), lba_io[3]);
        IDE_write_register(channel, ATA_REG_LBA4(0x0), lba_io[4]);
        IDE_write_register(channel, ATA_REG_LBA5(0x0), lba_io[5]);
    }
    IDE_write_register(channel, ATA_REG_SECTOR_COUNT(0x0), nb_sects);
    IDE_write_register(channel, ATA_REG_LBA_LO(0x0), lba_io[0]);
    IDE_write_register(channel, ATA_REG_LBA_MI(0x0), lba_io[1]);
    IDE_write_register(channel, ATA_REG_LBA_HI(0x0), lba_io[2]);

    // (VI) Select the command and set it
    if (lba_mode == LBA_MODE_CHS && dma == 0 && direction == ATA_READ) cmd = READ_PIO;
    if (lba_mode == LBA_MODE_28 && dma == 0 && direction == ATA_READ) cmd = READ_PIO;
    if (lba_mode == LBA_MODE_48 && dma == 0 && direction == ATA_READ) cmd = READ_PIO_EXT;
    if (lba_mode == LBA_MODE_CHS && dma == 1 && direction == ATA_READ) cmd = READ_DMA;
    if (lba_mode == LBA_MODE_28 && dma == 1 && direction == ATA_READ) cmd = READ_DMA;
    if (lba_mode == LBA_MODE_48 && dma == 1 && direction == ATA_READ) cmd = READ_DMA_EXT;
    if (lba_mode == LBA_MODE_CHS && dma == 0 && direction == ATA_WRITE) cmd = WRITE_PIO;
    if (lba_mode == LBA_MODE_28 && dma == 0 && direction == ATA_WRITE) cmd = WRITE_PIO;
    if (lba_mode == LBA_MODE_48 && dma == 0 && direction == ATA_WRITE) cmd = WRITE_PIO_EXT;
    if (lba_mode == LBA_MODE_CHS && dma == 1 && direction == ATA_WRITE) cmd = WRITE_DMA;
    if (lba_mode == LBA_MODE_28 && dma == 1 && direction == ATA_WRITE) cmd = WRITE_DMA;
    if (lba_mode == LBA_MODE_48 && dma == 1 && direction == ATA_WRITE) cmd = WRITE_DMA_EXT;

    IDE_write_register(channel, ATA_REG_COMMAND(0x0), cmd);

    if (dma)
    {
        if (direction == ATA_READ)
            {}// DMA Read
        else
            {}// DMA Write
    }
    else if (direction == ATA_READ)
    {
        // PIO Read
        for (i = 0; i < nb_sects; i++)
        {
            if ((err = IDE_polling(channel, 1)))
                return err;     // Polling, set error and exit if there is

            // Save es segment and repeat insw(read stream of shorts) instruction until nb_sects are read into buffer
            asm("pushw %es");
            asm("rep insw" :  : "c"(words), "d"(bus), "D"(buffer));     // Receive data
            asm("popw %es");
            buffer += (words * 2);
        }
    }
    else
    {
         // PIO Write.
        for (i = 0; i < nb_sects; i++) {
            IDE_polling(channel, 0);  // Polling.

            // Save es segment and repeat outsw(write stream of shorts) instruction util nb_sects are written to ide device
            asm("pushw %ds");
            asm("rep outsw" : : "c"(words), "d"(bus), "S"(buffer));  // Send Data
            asm("popw %ds");
            buffer += (words * 2);
        }
        // send the flush commands
        IDE_write_register(channel, ATA_REG_COMMAND(0x0), (char[]){CACHE_FLUSH, CACHE_FLUSH, CACHE_FLUSH_EXT}[lba_mode]);
        IDE_polling(channel, 0);  // Polling.
    }

    return 0;

}

void IDE_wait_irq(void)
{
    while(!ide_irq_invoked)
        ; // Wait for an IRQ
    ide_irq_invoked = 0;
}

void IDE_irq(void)
{
    ide_irq_invoked = 1;
}

/**
 * Read from ATAPI drive
 * - drive is the drive number, which is from 0 to 3
 * - lba is the LBA address
 * - nb_sects is the number of sectors. It should always be 1,
 * in case of reading more than one sectors, re-execute this function
 * with the updated LBA address
 * - selector is the segment selector
 * - edi is the offset of the selector
*/
/*
u8 IDE_ATAPI_read(u8 drive, u32 lba, u8 nb_sects, u16 selector, u32 edi)
{
    u8 channel = ide_devices[drive].channel;
    u8 slavebits = ide_devices[drive].drive;
    u16 bus = ide_channels[channel].base;
    u32 words = 1024; // Sector size. ATAPI drives have a sector siez of 2048 bytes
    u8 err;
    int i;

    // Enable IRQs
    IDE_write_register(channel, ATA_REG_CONTROL(0x0), ide_channels[channel].no_int = ide_irq_invoked = 0x0);

    // (I) Setup SCSI packet
    scsi_packet.op_code = READ_12;
    scsi_packet.flags_lo = 0x0;
    scsi_packet.lba_hi = (lba >> 24) & 0xFF;
    scsi_packet.lba_mihi = (lba >> 16) & 0xFF;
    scsi_packet.lba_milo = (lba >> 8) & 0xFF;
    scsi_packet.lba_lo = (lba >> 0) & 0xFF;
    scsi_packet.transfer_length_hi = 0x0;
    scsi_packet.transfer_length_mihi = 0x0;
    scsi_packet.transfer_length_milo = 0x0;
    scsi_packet.transfer_length_lo = nb_sects;
    scsi_packet.flags_hi = 0x0;
    scsi_packet.control = 0x0;

    // Select the drive
    IDE_write_register(channel, ATA_REG_DRIVE(0x0), slavebits << 4);

    // Delay 400ns for select to complete
    for (i = 0; i < 4; i++)
        IDE_read_register(channel, ATA_REG_ALTSTATUS(0x0)); // Reading the Alternate status ports wastes 100ns

    // (IV) Inform the controller that we use PIO mode
    IDE_write_register(channel, ATA_REG_FEATURES(0x0), 0);  // PIO mode

    // (V) Tell the controller the size of buffer
    IDE_write_register(channel, ATA_REG_LBA_MI(0x0), (words * 2) & 0xFF);  // Lower byte of sector size
    IDE_write_register(channel, ATA_REG_LBA_HI(0x0), (words * 2) >> 8);    // Upper byte of sector size

    // (VI) Send the Packet Command
    IDE_write_register(channel, ATA_REG_COMMAND(0x0), PACKET);

    // (VII) Waiting for the driver to finish and return an error code
    if ((err = IDE_polling(channel, 1)))
        return err;     // Polling and return if there is an error

    // (VIII) Sending the packet data
    asm ("rep outsw" :: "c"(6), "d"(bus), "S"(scsi_packet));

    // (IX) Receiving data
    // Here we should wait for an IRQ, then read the sector
    // These two operations should be repeated for each sector
    for (i = 0; i < nb_sects; i++)
    {
        IDE_wait_irq();     // Wait for an IRQ
        if ((err = IDE_polling(channel, 1)))
            return err;
        asm("pushw %es");
        asm("mov %%ax, %%es"::"a"(selector));
        asm("rep insw"::"c"(words), "d"(bus), "D"(edi));// Receive Data.
        asm("popw %es");
        edi += (words * 2);
    }

    // (X) Wait for an IRQ
    IDE_wait_irq();

    // (XI) Waiting for BSY and DRQ to clear
    while (IDE_read_register(channel, ATA_REG_STATUS(0x0)) & (BSY | DRQ))
        ;

    return 0;
}
*/

int IDE_read_sectors(u8 drive, u8 nb_sects, u32 lba, u16 es, u32 edi)
{
    // 1: Check if the drive presents
    if (drive > MAXIMUM_IDE_DEVICES || ide_devices[drive].reserved == 0)
    {
        console_printf("IDE Error: Drive not found\n");
        return -1;
    }
    // 2: Check if inputs are valid
    else if (((lba + nb_sects) > ide_devices[drive].size) && (ide_devices[drive].type == IDE_ATA))
    {
        console_printf("IDE Eroor: LBA adrress (0x%x) is greater than the available drive sectors(0x%x)\n", lba, ide_devices[drive].size);
        return -2;
    }
    // 3: Read in PIO mode through Polling and IRQs
    else
    {
        u8 err;
        if (ide_devices[drive].type == IDE_ATA)
            err = IDE_ATA_access(ATA_WRITE, drive, lba, nb_sects, edi);
        // else if (ide_devices[drive].type == IDE_ATAPI)
        //     for (u8 i = 0; i < nb_sects; i++)
        //         err = IDE_ATAPI_read(drive, lba + i, 1, es, edi + (i*2048));
        return IDE_print_error(drive, err);
    }
    return 0;
}

int IDE_write_sectors(u8 drive, u8 nb_sects, u32 lba, u16 es, u32 edi)
{
    (void)es;
    // 1: Check if the drive presents
    if (drive > MAXIMUM_IDE_DEVICES || ide_devices[drive].reserved == 0)
    {
        console_printf("IDE Error: Drive not found\n");
        return -1;
    }
    // 2: Check if inputs are valid
    else if (((lba + nb_sects) > ide_devices[drive].size) && (ide_devices[drive].type == IDE_ATA))
    {
        console_printf("IDE Eroor: LBA adrress (0x%x) is greater than the available drive sectors(0x%x)\n", lba, ide_devices[drive].size);
        return -2;
    }
    // 3: Read in PIO mode through Polling and IRQs
    else
    {
        u8 err;
        if (ide_devices[drive].type == IDE_ATA)
            err = IDE_ATA_access(ATA_WRITE, drive, lba, nb_sects, edi);
        // else if (ide_devices[drive].type == IDE_ATAPI)
        //     err = 4;    // Write-protected
        return IDE_print_error(drive, err);
    }
    return 0;
}

void init_ATA()
{
    init_IDE(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
}

int ATA_get_drive_by_model(const char *model)
{
     int i;
    for(i = 0; i < MAXIMUM_IDE_DEVICES; i++) {
        if(strcmp((const char*)ide_devices[i].model, (char *)model) == 0)
            return i;
    }
    return -1;
}