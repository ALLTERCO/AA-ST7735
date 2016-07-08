default: banner staticlib

BUILDDIR=build


OBJS=$(BUILDDIR)/Adafruit_ST7735.o

dirs:
	@if [ ! -d  $(BUILDDIR) ] ; then  mkdir -p $(BUILDDIR) ; fi;
clean:
	rm -f Adafruit_ST7735_c.a $(OBJS)
	
$(BUILDDIR)/%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

staticlib: dirs  Adafruit_ST7735_c.a

Adafruit_ST7735_c.a: $(OBJS)
	$(AR) $(ARFLAGS)  $@ $^
banner:
	@echo -ne "====================================\n=== CC VERSION: "`$(CC) --version | head -n 1`"\n====================================\n"
#dependancies:
$(BUILDDIR)/Adafruit_ST7735.o:Adafruit_ST7735.c Adafruit_ST7735_c.h Adafruit_ST7735_default_gfx.h Adafruit_GFX_proto.h Adafruit_GFX_body.h

