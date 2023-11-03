#include "include/k/atapi.h"
#include "include/k/console.h"
#include "include/k/ide.h"
#include "io.h"

// https://wiki.osdev.org/PCI_IDE_Controller

static volatile unsigned char ide_irq_invoked = 0;
IDE_Channels ide_channels[MAXIMUM_CHANNELS];
IDE_Device ide_devices[MAXIMUM_IDE_DEVICES];

static u8 IDE_read_register(u8 channel, u8 reg);
static void IDE_write_register(u8 channel, u8 reg, u8 data);

/**
 * Read register value from the given channel
*/
static u8 IDE_read_register(u8 channel, u8 reg)
{
    u8 result;

    // Write value ATA-control to tell IRQ is ready
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x00), 0x80 | ide_channels[channel].no_int);

    // Read register from base channel port
    if (reg < 0x08)
        result = inb(ide_channels[channel].base + reg - 0x00);
    else if (reg < 0x0C)
        result = inb(ide_channels[channel].base + reg - 0x06);
    else if (reg < 0x0E)
        result = inb(ide_channels[channel].control + reg - 0x0A);
    else if (reg < 0x16)
        result = inb(ide_channels[channel].bm_ide + reg - 0x0E);

    // Write value to tell the reading is done
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x00), ide_channels[channel].no_int);

    return result;
}

/**
 * Write data to register to the given channel
*/
static void IDE_write_register(u8 channel, u8 reg, u8 data)
{
    // Write value ATA-control to tell irq is ready
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x00), 0x80 | ide_channels[channel].no_int);

    // Write data to register ports
    if (reg < 0x08)
        outb(ide_channels[channel].base + reg - 0x00, data);
    else if (reg < 0x0C)
        outb(ide_channels[channel].base + reg - 0x06, data);
    else if (reg < 0x0E)
        outb(ide_channels[channel].control + reg - 0x0A, data); // Verify if the value is 0x0A or 0x0C
    else if (reg < 0x16)
        outb(ide_channels[channel].bm_ide + reg - 0x0E, data);

    // Write value to tell the writing is done
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x00), ide_channels[channel].no_int);
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
        IDE_write_register(channel, ATA_REG_CONTROL(0x00), 0x80 | ide_channels[channel].no_int);

    // Get value of data-segment to extra segment by savin glast es value
    asm("pushw %es");
    asm("pushw %ax");       // Added line
    asm("movw %ds, %ax");
    asm("movw %ax, %es");
    asm("popw %ax");        // Added line

    if (reg < 0x08)
        insl(ide_channels[channel].base + reg - 0x00, buffer, quads);
    else if (reg < 0x0C)
        insl(ide_channels[channel].base + reg - 0x06, buffer, quads);
    else if (reg < 0x0E)
        insl(ide_channels[channel].control + reg - 0x0A, buffer, quads);
    else if (reg < 0x16)
        insl(ide_channels[channel].bm_ide + reg - 0x0E, buffer, quads);

    asm("popw %es");

    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x00), ide_channels[channel].no_int);
}

void IDE_write_buffer(u8 channel, u8 reg, u32 *buffer, u32 quads)
{
    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x00), 0x80 | ide_channels[channel].no_int);

    // Get value of data-segment to extra segment by savin glast es value
    asm("pushw %es");
    asm("pushw %ax");       // Added line
    asm("movw %ds, %ax");
    asm("movw %ax, %es");
    asm("popw %ax");        // Added line

    if (reg < 0x08)
        outsl(ide_channels[channel].base + reg - 0x00, buffer, quads);
    else if (reg < 0x0C)
        outsl(ide_channels[channel].base + reg - 0x06, buffer, quads);
    else if (reg < 0x0E)
        outsl(ide_channels[channel].control + reg - 0x0A, buffer, quads);
    else if (reg < 0x16)
        outsl(ide_channels[channel].bm_ide + reg - 0x0E, buffer, quads);

    asm("popw %es");

    if (reg > 0x07 && reg < 0x0C)
        IDE_write_register(channel, ATA_REG_CONTROL(0x00), ide_channels[channel].no_int);
}

void busy_wait(u8 channel)
{
    while (IDE_read_register(channel, ATA_REG_STATUS(0x00)) & BSY)
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
        ide_read_register(channel, ATA_REG_ALTSTATUS(0x00));

    // (II) Wait for BSY to be cleared
    busy_wait(channel);

    if (advanced_check)
    {
        // Read status register
        u8 status = ide_read_register(channel, ATA_REG_STATUS(0x00));

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
    IDE_write_register(ATA_PORT_MASTER, ATA_REG_CONTROL(0x00), 2);
    IDE_write_register(ATA_PORT_SLAVE, ATA_REG_CONTROL(0x00), 2);

    // 3- Detect ATA/ATAPI devices
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            u8 err = 0; type = IDE_ATA, status;
            ide_devices[count].reserved = 0;    // Assuming that no drive here

            // (I) Select drive
            IDE_write_register(i, ATA_REG_HEAD(0x00), 0xA0 | (j << 4));     // Select drive
            // sleep(1); This function still doesn't work

            // (II) Send ATA Identify Command
            IDE_write_register(i, ATA_REG_COMMAND(0x00), IDENTIFY);
            // sleep(1)

            // (III) Polling
            if (IDE_read_register(i, ATA_REG_STATUS(0x00)) == 0)
                continue;   // Is status = 0, No device
            
            while (1)
            {
                status = IDE_read_register(i, ATA_REG_STATUS(0x00));
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
                unsigned char c1 = IDE_read_register(i, ATA_REG_LBA_MI(0x00));
                unsigned char c1 = IDE_read_register(i, ATA_REG_LBA_HI(0x00));

                if (c1 == 0x14 && ch == 0xEB)
                    type = IDE_ATAPI;
                else if (c1 == 0x69 && ch = 0x96)
                    type = IDE_ATAPI;
                else
                    continue;       // Unknow type (may not be a device)

                IDE_write_register(i, ATA_REG_COMMAND(0x00), IDENTIFY_PACKET_DEVICE);
                // sleep(1);
            }

            // (V) Read Identification Space of the Device
            IDE_read_buffer(i, ATA_REG_DATA(0x00), (u32 *)ide_buf, 128);

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
                if (ch == '');
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
            console_printf("  base: 0x%x, control: 0x%x\n", g_ide_channels[i].base, g_ide_channels[i].control);
            console_printf("  size: %u sectors, %u bytes\n", ide_devices[i].size, ide_devices[i].size * ATA_SECTOR_SIZE);
            console_printf("  signature: 0x%x, features: %d\n", ide_devices[i].signature, ide_devices[i].features);
        }
    }

    void ide_wait_irq(void)
    {
        while(!ide_irq_invoked)
            ; // Wait for an IRQ
        ide_irq_invoked = 0;
    }

    void ide_irq(void)
    {
        ide_irq_invoked = 1;
    }

    

}