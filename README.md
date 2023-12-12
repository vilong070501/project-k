# PROJECT K

This is a project implementing a simple kernel with some basic functionalities.

The project is proposed by [LSE Epita](https://github.com/lse/k).

Student's name: LUONG Vi Long (vi-long.luong@epita.fr)

# Table of contents

1. [Installation](#installation)
   - [Dependencies](#dependencies-for-building)
2. [Build System](#build-system)
   - [Boost kernel](#booting-your-kernel-in-qemu)
3. [Available modules](#available-modules)
4. [How to test](#test-different-module)
5. [References](#references)

# Installation

## Dependencies for building

* gcc-multilib
* grub2
* libisoburn
* find
* qemu-system-i386
* xorriso
* mtools

# Build System

Here are make rules you need to know:

    make | make k.iso  # create an ISO with all the roms
    make k             # compile your kernel
    make rom/GAME      # compile the rom in the folder rom/$(GAME)
    make clean         # clean the tree

## Booting your kernel in qemu

    qemu-system-i386 -cdrom k.iso [ -enable-kvm ]
    qemu-system-i386 -cdrom k.iso -serial stdio    # for displaying the debuging printf on the terminal

# Available modules:
- `write()` function in order to print text on your terminal, implemented in **k/write.c** file. This function makes use of the `outb()` function,
which is an assembler instruction. You can find a list of basic assembler instructions implemented in C in **k/io.h** file.


- `console` driver, which allows you to display text on the screen when operating in text mode. It initializes the screen with two colors
black and white, and possesses some functions to manipulate the screen, especially the function `console_printf()` having the same behavior as
`printf()`. You will find the code of this module in **k/console.c** file.


- `GDT` module, which is in charge of building a **Global Descriptor Table** (GDT, an array that contains the information about every segment), and then
load it to the kernel. When initializing the GDT, a **Task State Segment** (TSS) is also loaded to the 6<sup>th</sup> entry of the array in order to
switch from *userland* to *kernelland*. You will find the code of this module in **k/gdt.c** file.


- `IDT` module, which is in charge of building an **Interrupt Descriptor Table** (IDT, an array that contains the information about interrupts handling
by processor). This is a way to tell the CPU where the **Interrupt Service Routines** (ISR) are located. You will find the code of this module
in **k/idt.c** file.


- `ISR` module, which stands for **Interrupt Service Routines**. An ISR is a handler which handles interrupt triggered by the hardware or software. 
For the system to know which interrupt service routine to call when a certain interrupt occurs, offsets to the ISRs are stored in the IDT.
You will find the code of this module in **k/isr.c** file.


- `IRQ` module, which stands for **Interrupt Request**. When a device needs to inform the system (i.e.: when new data is available,
or on completion of a task), it fires an Interrupt Request. An example of an IRQ is the keyboard: everytime you press a key, the keyboard triggers IRQ1,
and the corresponding interrupt handler is called. You will find the code of this module in **k/irq.c** file.


- `PIC` module, which stands for **Programmable Interrupt Controller**, one of the most important chips of the x86 architecture which manages hardware
interrupts and send them to the appropriate system interrupt. When an IRQ is fired, the PIC decides to deliver the interrupt to the CPU (if the
interrupt is not masked), and find out the number of the IDT entry to call. You will find the code of this module in **k/pic_8259.c** file.


- `Timer` driver, which handles time intervals in the kernel. This driver does not provide real date and time calculation routines. It is rather used
as a rate generator, to wake the kernel up at accurate intervals of time, measured in ticks. You will find the code of this module in **k/timer.c** file.


- `Keyboard` driver, which used to add a first form of input in the project K. This driver detects when a key is pressed by configuring the keyboard
controller to trigger interrupt when a key is pressed. It then displays the corresponded character to the screen by looking into a mapping array,
which associates each key code to its character value. You will find the code of this module in **k/keyboard.c** file.


- `Mouse` driver, which is another form of input. This driver detects when one of the three mouse's keys is pressed (can be simultaneously), or when
we move the mouse around the screen. You will find the code of this module in **k/mouse.c** file.


- `ATA` driver, which stands for **Advanced Technology Attachment**. In this project, I had implemented the IDE (**Integrated Drive Electronics**),
which refers to a standard interface used to connect storage devices, such as hard drives and CD-ROM drives, to a computer's motherboard.
You will find the code of this module in **k/ide.c** file.

# Test Different Module

All the available modules are initialized inside the function `HAL_Initialize()` defined in **k/hal.c** file. To test each module, you just need to
uncomment the line of initializing (you could also test every module together).

By default, I will let uncommented the first 3 modules, `GDT` and `IDT` as they are required in order to boost the kernel correctly, and `console` in order
to display something to the screen.

1. ISR, Exceptions handler:

In order to test the loading of the IDT into the system and declaration of some ISR, I have added an example which will raise a "Division by Zero"
exception. First, go to the **k/hal.c** file, uncomment these lines:
```C
25|      init_ISR();
26|      console_printf("ISR initialization finished\n");
```
and in the **k/k.c** file, uncomment one of these two examples:

```C
51|      // Raise Division by zero exception
52|      asm volatile ("mov $0, %eax");
53|      asm volatile ("div %eax");
54|      asm volatile("ret");

Or

56|      // Raise another Division by zero exception due to Overflow
57|      asm volatile ("mov 0xffffffff, %eax");
58|      asm volatile ("mov 0xffffffff, %edx");
59|      asm volatile ("mov $0x2, %ebx");
60|      asm volatile ("div %ebx");
61|      asm volatile("ret");
```
Then, `make` and boost your kernel to see the exception.

2. Timer:

In order to test the Timer manager in K, you have to register `timer_handler()` function to the first entry of IDT. I also prepared for you two test functions `function_1()` and `function_2()` in **k/k.c** to visualize how `Timer` driver works.

Go to **k/hal.c** file and uncomment these lines:
```C
25|      init_ISR();
26|      console_printf("ISR initialization finished\n");
27|      init_IRQ();
28|      console_printf("IRQ initialization finished\n");
29|      init_timer();
30|      console_printf("Timer initialization finished\n");
```
and in the **k/k.c** file, uncomment these two lines:
```C
64|      add_timer_function(function_1, 200);
65|      add_timer_function(function_2, 300);
```
Then, `make` and boost your kernel to see how `Timer` driver works.

3. Keyboard:

In order to test the Keyboard manager in K, you have to register `keyboard_handler()` function to the second entry of IDT.

To do so, go to **k/hal.c** file and uncomment these lines:
```C
25|      init_ISR();
26|      console_printf("ISR initialization finished\n");
27|      init_IRQ();
28|      console_printf("IRQ initialization finished\n");
29|      init_timer();
30|      console_printf("Timer initialization finished\n");
31|      vga_disable_cursor();
32|      init_keyboard();
33|      console_printf("Keyboard initialization finished\n");
```
and in the **k/k.c** file, uncomment these lines:
```C
68|      console_printf("Type something...\n");
69|
70|      while(1)
71|      {
72|          console_printf("%c", keyboard_getChar());
73|      }
```
Then, `make` and boost your kernel to see how `Keyboard` driver works.

4. Mouse:

In order to test the Mouse manager in K, you have to register `mouse_handler()` function to the 13<sup>th</sup> entry of IDT.

To do so, go to **k/hal.c** file and uncomment these lines:
```C
25|      init_ISR();
26|      console_printf("ISR initialization finished\n");
27|      init_IRQ();
28|      console_printf("IRQ initialization finished\n");
29|      init_timer();
30|      console_printf("Timer initialization finished\n");
31|      vga_disable_cursor();
32|      init_keyboard();
33|      console_printf("Keyboard initialization finished\n");
34|      init_mouse();
35|      console_printf("Mouse initialization finished\n");
```
`make` and boost your kernel. Once the kernel boost, click any mouse button and try to move your mouse around. You will see that
the coordinates of the cursor are updated when the mouse move.

5. ATA:

In order to test the ATA driver in K, uncomment these lines in the **k/hal.c** file:
```C
25|      init_ISR();
26|      console_printf("ISR initialization finished\n");
27|      init_IRQ();
28|      console_printf("IRQ initialization finished\n");
29|      init_timer();
30|      console_printf("Timer initialization finished\n");
31|      vga_disable_cursor();
32|      init_keyboard();
33|      console_printf("Keyboard initialization finished\n");
34|      init_mouse();
35|      console_printf("Mouse initialization finished\n");
36|      init_ATA();
37|      console_printf("ATA initialization finished\n");
```
Then `make` and boost your system. You will see some information related to the actual drive of K.

6. VGA graphics mode:

In order to test the VGA graphics mode in K

Firstly, in **k/console.c**, comment this line 
`16|     VGA_COLOR_TYPE fore_color = COLOR_WHITE_TEXT, back_color = COLOR_BLACK;`
and uncomment this line
`15|     VGA_COLOR_TYPE fore_color = COLOR_WHITE, back_color = COLOR_BLACK;`

To test the change of color in graphics mode, we will use the mouse driver.

Go to **k/mouse.c** and uncomment these lines
```C
155|     clear_console(COLOR_WHITE_TEXT, COLOR_BLACK);
156|     console_gotoxy(mouse_x_pos, mouse_y_pos);
157|     console_putchar('X');
158|     print_mouse_info();
```
and uncomment these lines:
```C
160|     VGA_graphics_fill_color(COLOR_WHITE);
161|     VGA_graphics_draw_rect(mouse_x_pos, mouse_y_pos, 10, 10, COLOR_BLUE);
162|     print_mouse_info_VGA();
```
In the **k/hal.c** file, comment this line 
`18|     init_console(COLOR_WHITE_TEXT, COLOR_BLACK);`
and uncomment theses lines
```C
17|     init_console(COLOR_WHITE, COLOR_BLACK);

25|      init_ISR();
26|      console_printf("ISR initialization finished\n");
27|      init_IRQ();
28|      console_printf("IRQ initialization finished\n");
29|      init_timer();
30|      console_printf("Timer initialization finished\n");
31|      vga_disable_cursor();
32|      init_keyboard();
33|      console_printf("Keyboard initialization finished\n");
34|      init_mouse();
35|      console_printf("Mouse initialization finished\n");

38|      init_VGA_graphics();
39|      console_printf("VGA graphic initialization finished\n");
```
Finally, in **k/k.c**, uncomment these two lines
```C
78|     VGA_graphics_fill_color(COLOR_WHITE);
79|     VGA_graphics_draw_rect(10, 10, 10, 10, COLOR_GREEN);
```
Then `make`. Once you boost your system, you will see a white screen with a little green
rectangle representing the mouse. Left lick the mouse over and over again to see the
change of color. To get back to the previous color, right click it.

# References

[[1]](https://github.com/lse/k) Source git repository of the project given by the Epita LSE

[[2]](https://wiki.osdev.org/Main_Page) Wikipedia page of OS Development

[[3]](http://www.intel.com/products/processor/manuals/) Intel Manuals for OS Development

