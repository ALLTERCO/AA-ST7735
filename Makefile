default: banner staticlib

BUILDDIR=build


OBJS=$(BUILDDIR)/AA_ST7735.o

dirs:
	@if [ ! -d  $(BUILDDIR) ] ; then  mkdir -p $(BUILDDIR) ; fi;
clean:
	rm -f AA_ST7735_c.a $(OBJS)
	
$(BUILDDIR)/%.o:%.c
	$(CC) $(CFLAGS) -Wall -c -o $@ $<

staticlib: dirs  AA_ST7735_c.a

AA_ST7735_c.a: $(OBJS)
	$(AR) $(ARFLAGS)  $@ $^
banner:
	@echo -ne "====================================\n=== CC VERSION: "`$(CC) --version | head -n 1`"\n====================================\n"
#dependancies:
$(BUILDDIR)/AA_ST7735.o:AA_ST7735.c AA_ST7735.h AA_ST7735_default_gfx.h AA-GFX/AA_GFX_proto.h AA-GFX/AA_GFX_body.h

