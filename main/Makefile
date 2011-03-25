PRG            = reflow
OBJ            = main.o lcd.o eeprom.o max6675.o enc.o serial.o pid.o ramp.o menu.o pwm.o
MCU_TARGET     = atmega8
OPTIMIZE       = -Os
#LDFLAGS += -T

# You should not have to change anything below here.

CC             = avr-gcc
SIZE           = avr-size

# Override is only needed by avr-lib build system.

override CFLAGS = -std=c99 -Winline -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: hex
	$(SIZE) $(PRG).elf

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf $(PRG).hex

hex:  $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

ifdef windir
UISPFLAGS := -dlpt=0x378
endif

#
#load: $(PRG).hex
#	uisp -dlpt=0x378 -dprog=stk200 --wr_fuse_l=0xff --wr_fuse_h=0xc9 --erase --upload --verify if=$< $(UISPFLAGS)
load: $(PRG).hex
#	avrdude -P /dev/parport0 -c stk200 -p m16 -U flash:w:main.hex
#	avrdude -c buspirate -P /dev/ttyUSB0 -p $(MCU_TARGET) -U flash:w:$(PRG).hex
	avrdude -c avr109 -P /dev/ttyUSB0 -p m8 -b 19200 -p $(MCU_TARGET) -U flash:w:$(PRG).hex
