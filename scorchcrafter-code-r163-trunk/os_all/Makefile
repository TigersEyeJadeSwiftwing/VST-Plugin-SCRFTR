BINSUITE =
BINC120 =
BINGLZ60 =
BINJYK =
BINGUITARIST =
BINDRUMMER =
BINPKWRT =
BINTOOLPATH =

SLOTS = 7

COMPILERPATH =
COMPILERPREFIX =
COMPILERC =
COMPILERCPP =
RESOURCETOOL =
LINKERTOOL =

GUIV =
GUILIBS =

BUILDLOGPATH = ./build_logs/
BLDLOGFILE = log.txt
LGS = $(BUILDLOGPATH)$(BLDLOGFILE)
ADDCFLAGS = -static-libgcc
ADDCPPFLAGS =
STATICFLAGS = -static-libstdc++ -static-libgcc

CFLAGS = -trigraphs -m128bit-long-double -Wno-multichar -Wno-write-strings -Wno-pointer-arith

CPPFLAGS = $(CFLAGS) -ffinite-math-only -fno-signed-zeros -fno-trapping-math -fno-exceptions

SSE = 0
SSEINFO =
ifeq ($(SSE),0)
SSEINFO += Not using SSE -Default- This is usually the best setting for ScorchCrafter software.
endif
ifeq ($(SSE),2)
CPPFLAGS += -mfpmath=sse -msse2
SSEINFO += Using SSE 2
endif
ifeq ($(SSE),3)
CPPFLAGS += -mfpmath=sse -mssse3
SSEINFO += Using SSSE 3
endif
ifeq ($(SSE),4)
CPPFLAGS += -mfpmath=sse -msse4.2
SSEINFO += Using SSE 4.2
endif

BLDC = $(CFLAGS) $(ADDCFLAGS)
BLDCPP = $(CPPFLAGS) $(ADDCPPFLAGS)

CDEFS = -DScC120_EXPORTS -DPlgEditor_EXPORTS -DWINDOWS -D_WINDOWS -DWIN32 -D_USERDLL \
	-D_USE_MATH_DEFINES -DUSE_LIBPNG=1 -DCRT_SECURE_NO_WARNINGS=1 \
	-D_CRT_SECURE_NO_WARNINGS=1 -DCRT_SECURE_NO_DEPRECATE=1 -D_CRT_SECURE_NO_DEPRECATE=1 -DNDEBUG \
	-DEXPORTS -DPNG_NO_MNG_FEATURES -DPNG_STATIC -DMINGW32 -D_MINGW32_ -D__MINGW32__ -DGNUC -D_GNUC_ \
	-DZLIB_EXPORTS -DLIBZ_EXPORTS -DZ_EXPORTS -DLIBPNG_EXPORTS

CINCLUDES = -I./ -I./sdk_vst -I./vst_c120 -I./sdk_vst/pluginterfaces/vst2.x \
	-I./sdk_vst/public.sdk/source/vst2.x -I./common -I./zlib -I./libpng -I./common/eq \
	-I./common/dist -I./common/eq/wavs_amp -I./common/un -I./vst_jykkr -I./vsti_guitarist \
	-I./common/synth -I./common/impulse -I./vsti_drummer -I./peakwriter -I./common_c

CARCH =

UNOBJGRAPHICS = un_lib_zlib.o un_lib_libpng.o

OBJGRAPHICS = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o gzwrite.o \
	infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o \
	png.o pngerror.o pngget.o pngmem.o pngpread.o  pngread.o pngrio.o \
	pngrtran.o pngrutil.o pngset.o pngtrans.o pngwio.o pngwrite.o pngwtran.o pngwutil.o

OBJZLIB = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o gzwrite.o \
	infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o

OBJLIBPNG = png.o pngerror.o pngget.o pngmem.o pngpread.o  pngread.o pngrio.o \
	pngrtran.o pngrutil.o pngset.o pngtrans.o pngwio.o pngwrite.o pngwtran.o pngwutil.o

OBJVSTSDK = un_sdk_vst.o

ifeq ($(GUIV),4)
CDEFS += -DUSE_VER_FOUR_GUI
UNOBJVSTGUI = un_sdk_vstgui4.o
OBJVSTGUI = un_sdk_vstgui4.o
CINCLUDES += -I./sdk_vstgui4 -I./sdk_vstgui4/lib -I./sdk_vstgui4/lib/controls -I./sdk_vstgui4/lib/animation \
	-I./sdk_vstgui4/plugin-bindings
else
UNOBJVSTGUI = un_sdk_vstgui.o
OBJVSTGUI = vstgui.o aeffguieditor.o plugguieditor.o vstcontrols.o
CINCLUDES += -I./sdk_vstgui
endif

OBJVSTSUITE = suite.o un_vst_suite.o suite_gui.o

OBJVSTC120 = c120.o c120_gui.o un_vst_c120.o
OBJVSTGLZ60 = glz60.o glz60_gui.o un_vst_glz60.o
OBJVSTJYK = jyk.o jyk_gui.o un_vst_jykkr.o
OBJVSTGUITARIST = guitarist.o un_vsti_guitarist.o
OBJVSTDRUMMER = drummer.o un_vsti_drummer.o

LIBS = -luuid -lshell32 -lole32 -lUser32 -ladvapi32

ZLIBFLAGS = $(BLDC) $(CARCH) $(CDEFS) -DNO_VIZ
ZLIBHEADERS =

PNGFLAGS = $(ZLIBFLAGS)
PNGHEADERS =

all: tg-info \
	help

graphicobj:
	$(COMPILERC) $(CFLAGS) $(CARCH) $(CDEFS) $(SRCOBJGRAPHICS) -o gobj

un-graphicsrc:
	@echo -e "Compiling... \r" >> $(LGS)
	@echo Compiling...
	@echo -e "zlib... \r" >> $(LGS)
	@echo zlib...
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./common/un/un_lib_zlib.c >> $(LGS)
	@echo -e "libpng...\r" >> $(LGS)
	@echo libpng...
	@$(COMPILERC) $(PNGFLAGS) -O3 $(CINCLUDES) -c ./common/un/un_lib_libpng.c >> $(LGS)

graphicsrc:
	@echo -e "Compiling... \r" >> $(LGS)
	@echo Compiling...
	@echo -e "zlib... \r" >> $(LGS)
	@echo zlib...
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/adler32.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/compress.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/crc32.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/deflate.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/gzclose.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/gzlib.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/gzread.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/gzwrite.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/infback.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/inffast.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/inflate.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/inftrees.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/trees.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/uncompr.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O3 -w $(CINCLUDES) -c ./zlib/zutil.c $(ZLIBHEADERS)>> $(LGS)
	@echo -e "libpng...\r" >> $(LGS)
	@echo libpng...
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/png.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngerror.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngget.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngmem.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngset.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngpread.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngread.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrio.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrtran.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrutil.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngtrans.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwio.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwrite.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwtran.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwutil.c $(PNGHEADERS)>> $(LGS)

cm-vstsdk:
	@echo -e "vst SDK, 2.4, r2... \r" >> $(LGS)
	@echo vst SDK, 2.4, r2...
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) \
	-c ./sdk_vst/pluginterfaces/vst2.x/aeffect.h ./sdk_vst/public.sdk/source/vst2.x/audioeffect.cpp>> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) \
	-c ./sdk_vst/pluginterfaces/vst2.x/aeffectx.h ./sdk_vst/public.sdk/source/vst2.x/audioeffectx.cpp>> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) \
	-c ./sdk_vst/public.sdk/source/vst2.x/vstplugmain.cpp>> $(LGS)

un-vstsdk:
	@echo -e "vst SDK, 2.4, r2... \r" >> $(LGS)
	@echo vst SDK, 2.4, r2...
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./common/un/un_sdk_vst.cpp >> $(LGS)

cm-vstgui:
	@echo -e "vst GUI... \r" >> $(LGS)
	@echo vst GUI...
ifeq ($(GUIV),4)
	@echo -e "(Using v4, from SVN snapshot) \r" >> $(LGS)
	@echo "(Using v4, from SVN snapshot)"
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./common/un/un_sdk_vstgui4.cpp >> $(LGS)
else
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./sdk_vstgui/vstgui.cpp $(GUILIBS) >> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./sdk_vstgui/aeffguieditor.cpp $(GUILIBS) >> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./sdk_vstgui/plugguieditor.cpp $(GUILIBS) >> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./sdk_vstgui/vstcontrols.cpp $(GUILIBS) >> $(LGS)
endif

un-vstgui:
	@echo -e "vst GUI... \r" >> $(LGS)
	@echo vst GUI...
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./common/un/un_sdk_vstgui.cpp >> $(LGS)

cm-c120-intfc:
	@echo -e "C120 VST Interface code \r" >> $(LGS)
	@echo C120 VST Interface code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) -c ./vst_c120/c120.cpp >> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) -I./common -c ./vst_c120/c120_gui.cpp >> $(LGS)

cm-c120:
	@echo -e "C120 VST Plugin, actual code \r">> $(LGS)
	@echo C120 VST Plugin, actual code
	@rm -rf ./common/plugin_c120.gch
	@rm -rf ./tube_bank_d.o
	@rm -rf ./tonestack_e.o
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) -DBUILDVST_C120 \
	-c ./common/custdata_types.cpp \
	./common/sample_bank_custom.cpp ./common/eq/csts_eq.cpp ./common/eq/lp_btw.cpp \
	./common/eq/tonestack_e.cpp ./common/dist/tube_bank_d.cpp \
	./common/plugin_c120.cpp >> $(LGS)

un-c120:
	@echo -e "C120 VST Plugin, actual code \r">> $(LGS)
	@echo C120 VST Plugin, actual code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./common/un/un_vst_c120.cpp >> $(LGS)

un-guitarist:
	@echo -e "Sc Guitarist, actual code \r">> $(LGS)
	@echo Sc Guitarist, actual code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) -c ./common/un/un_vsti_guitarist.cpp >> $(LGS)

cm-gtr-intfc:
	@echo -e "Guitarist VSTi Interface code \r" >> $(LGS)
	@echo Guitarist VSTi Interface code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./vsti_guitarist/guitarist.cpp >> $(LGS)
#	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -I./common -c ./vst_jykkr/jyk_gui.cpp >> $(LGS)

un-drummer:
	@echo -e "Sc Drummer, actual code \r">> $(LGS)
	@echo Sc Drummer, actual code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) -c ./common/un/un_vsti_drummer.cpp >> $(LGS)

cm-drmr-intfc:
	@echo -e "SC Drummer VSTi Interface code \r" >> $(LGS)
	@echo SC Drummer VSTi Interface code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./vsti_drummer/drummer.cpp >> $(LGS)
#	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -I./common -c ./vst_jykkr/jyk_gui.cpp >> $(LGS)

lk-c120:
	@echo -e "Creating Resource File... \r" >> $(LGS)
	@echo Creating Resource File...
	@$(RESOURCETOOL) -i ./vst_c120/c120.rc -J rc -o rc_c120.o -O coff >> $(LGS)
	@echo -e "Linking C120 VST... \r" >> $(LGS)
	@echo Linking C120 VST...
	@$(COMPILERCPP) -shared -Wl,--dll -s $(LIBS) \
	$(OBJGRAPHICS) $(OBJVSTGUI) $(OBJVSTSDK) $(OBJVSTC120) rc_c120.o \
	-o $(BINC120) --def ./vst_c120/c120.def $(CFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

cm-glz60-intfc:
	@echo -e "GLZ60 VST Interface code \r" >> $(LGS)
	@echo GLZ60 VST Interface code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./vst_glz60/glz60.cpp >> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -I./common -c ./vst_glz60/glz60_gui.cpp >> $(LGS)

cm-glz60:
	@echo -e "GLZ60 VST Plugin, actual code \r">> $(LGS)
	@echo GLZ60 VST Plugin, actual code
	@rm -rf ./common/plugin_glz60.gch
	@rm -rf ./tube_bank_d.o
	@rm -rf ./tonestack_e.o
	@rm -rf ./tonestack_f.o
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) -DBUILDVST_GLZ60 \
	-c ./common/custdata_types.cpp \
	./common/sample_bank_custom.cpp ./common/eq/csts_eq.cpp ./common/eq/lp_btw.cpp \
	./common/eq/tonestack_e.cpp ./common/eq/tonestack_f.cpp ./common/dist/tube_bank_d.cpp \
	./common/plugin_glz60.cpp >> $(LGS)

un-glz60:
	@echo -e "GLZ60 VST Plugin, actual code \r">> $(LGS)
	@echo GLZ60 VST Plugin, actual code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) -c ./common/un/un_vst_glz60.cpp >> $(LGS)

lk-glz60:
	@echo -e "Creating Resource File... \r" >> $(LGS)
	@echo Creating Resource File...
	@$(RESOURCETOOL) -i ./vst_glz60/glz60.rc -J rc -o rc_glz60.o -O coff >> $(LGS)
	@echo -e "Linking GLZ60 VST... \r" >> $(LGS)
	@echo Linking GLZ60 VST...
	@$(COMPILERCPP) -shared -Wl,--dll -s $(LIBS) \
	$(OBJGRAPHICS) $(OBJVSTGUI) $(OBJVSTSDK) $(OBJVSTGLZ60) rc_glz60.o \
	-o $(BINGLZ60) --def ./vst_glz60/glz60.def $(CFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

cm-jyk-intfc:
	@echo -e "Jykwrakker VST Interface code \r" >> $(LGS)
	@echo Jykwrakker VST Interface code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./vst_jykkr/jyk.cpp >> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -I./common -c ./vst_jykkr/jyk_gui.cpp >> $(LGS)

un-jyk:
	@echo -e "Jykwrakker VST Plugin, actual code \r">> $(LGS)
	@echo Jykwrakker VST Plugin, actual code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) -c ./common/un/un_vst_jykkr.cpp >> $(LGS)

lk-jyk:
	@echo -e "Creating Resource File... \r" >> $(LGS)
	@echo Creating Resource File...
	@$(RESOURCETOOL) -i ./vst_jykkr/jyk.rc -J rc -o rc_jyk.o -O coff >> $(LGS)
	@echo -e "Linking Jykwrakker VST... \r" >> $(LGS)
	@echo Linking Jykwrakker VST...
	@$(COMPILERCPP) -shared -Wl,--dll -s $(LIBS) \
	$(OBJGRAPHICS) $(OBJVSTGUI) $(OBJVSTSDK) $(OBJVSTJYK) rc_jyk.o \
	-o $(BINJYK) --def ./vst_jykkr/jyk.def $(CFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

clean-obj:
	@echo -e "Cleaning Up... \r" >> $(LGS)
	@echo Cleaning Up...
	@rm -rf *.o

clean: clean-obj
	@rm -rf rc_c120.0
	@rm -rf rc_glz60.0
	@rm -rf rc_jyk.o
	@rm -rf $(BINC120)
	@rm -rf $(BINGLZ60)
	@rm -rf $(BINJYK)

clear-logs:
	@echo -e Erasing Old Build Logs... \n
	@rm -rf $(LGS)
	@echo -e Saving build record to log file at $(LGS) \n

build-info:
	@echo -e "Compiler path is "$(COMPILERPATH) "\r" >> $(LGS)
	@echo -e "Using \r" >> $(LGS)
	@echo -e "  "$(COMPILERC) "\r" >> $(LGS)
	@echo -e "  "$(COMPILERCPP) "\r" >> $(LGS)
	@echo -e "  "$(RESOURCETOOL) "\r" >> $(LGS)
	@echo -e "Target Archetecture switches are \r  "$(CARCH) "\r" >> $(LGS)
	@echo -e Compiler path is $(COMPILERPATH) \n >> $(LGS)
	@echo -e $(SSEINFO) \n >> $(LGS)
	@echo -e Using \n
	@echo -e $(COMPILERC) \n
	@echo -e $(COMPILERCPP) \n
	@echo -e $(RESOURCETOOL) \n
	@echo -e Target Archetecture switches are $(CARCH) \n
	@echo -e Compiler path is $(COMPILERPATH) \n
	@echo -e $(SSEINFO) \n
	@echo -e ""

help:
	@echo "Some of the targets you can run MAKE with are:"
	@echo ""
	@echo "  clean:       Clean up files used for building stuff"
	@echo "  build-info:  Display default build options and flags for this Makefile"
	@echo "  help:        Display this text."
	@echo ""
	@echo "  deps-w32:   Build GUI graphic dependency libraries LibPNG and ZLIB (Windows),"
	@echo "      using MinGW-w64, 32-bit"
	@echo "  deps-w64:   Build GUI graphic dependency libraries LibPNG and ZLIB (Windows),"
	@echo "      using MinGW-w64, 64-bit"
	@echo "  deps-tdm32: Build GUI graphic dependency libraries LibPNG and ZLIB (Windows),"
	@echo "      using TDM MinGW, 32-bit"
	@echo "  deps-tdm64: Build GUI graphic dependency libraries LibPNG and ZLIB (Windows),"
	@echo "      using TDM MinGW, 64-bit"
	@echo ""
	@echo "  vst-w32:   Build Windows VST/VSTi DAW plug-ins, using MinGW-w64, 32-bit"
	@echo "  vst-w64:   Build Windows VST/VSTi DAW plug-ins, using MinGW-w64, 64-bit"
	@echo "  vst-tdm32: Build Windows VST/VSTi DAW plug-ins, using TDM MinGW, 32-bit"
	@echo "  vst-tdm64: Build Windows VST/VSTi DAW plug-ins, using TDM MinGW, 64-bit"
	@echo ""
	@echo "Options for building the Suite, which is a collection of many FX put into one"
	@echo "     single, compound plugin, with menus included in the"
	@echo "     interface (Different than the stand-alone versions):"
	@echo ""
	@echo "  vst-suite-w32:	Build Full Windows VST Suite, using MinGW-w64, 32-bit"
	@echo "  vst-suite-w64:	Build Full Windows VST Suite, using MinGW-w64, 64-bit"
	@echo "NOTE: You can specify 'SLOTS=(Some integer/whole number)' with these"
	@echo "    targets to determine how many slots are visible in the GUI, which"
	@echo "    of course changes how much space the GUI takes on the screen in"
	@echo "    your DAW's plug-in window.  Hint: Each GUI slot takes 100 vertical pixels."
	@echo ""
	@echo "It is recommended to rebuild graphic dependencies first, before other stuff,"
	@echo "  but if you have already done so and are using the same version of the same"
	@echo "  compiler over and over, this might not be necessary."
	@echo ""
	@echo "Build flags you might find useful:"
	@echo "  ADDCFLAGS - Additional gcc compile flags"
	@echo "  ADDCPPFLAGS - Additional g++ compile flags"
	@echo "  CARCH - gcc and g++ target archetecture switches"
	@echo "  COMPILERPATH - Path to your compiler"
	@echo "  COMPILERPREFIX - Used if your compiler's executables have additional"
	@echo "    characters in their names (such as 'i686-mingw32-gcc' instead of 'gcc')"
	@echo "  COMPILERC - Default is 'gcc'"
	@echo "  COMPILERCPP - Default is 'g++'"
	@echo "  RESOURCETOOL - Default is 'windres'"
	@echo ""
	@echo "You can also set SSE to '=' 2, 3, or 4 if you want to compile with SSE."
	@echo "  This isn't always recommended though, because it might make the 128-bit"
	@echo "  processing modes fail to work with full precision.  (The worst"
	@echo "  that would happen is *usually* increased CPU but audio that"
	@echo "  sounds as if it were using the 64-bit mode.)"
	@echo " 'SSE=2' --> SSE2, 'SSE=3' --> SSSE3, 'SSE=4' --> SSE 4.2"
	@echo ""
	@echo "You're also welcome to poke around in the actual Makefile with a text editor,"
	@echo "  If you think you know what you're doing."
	@echo ""

tg-info:
	@echo "Please select a build target"
	@echo "   and, if needed, set flags."
	@echo ""

make-deps:
	@echo "Now Rebuilding zlib and libpng"
	@echo "Compiling zlib..."
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/adler32.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/compress.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/crc32.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/deflate.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/gzclose.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/gzlib.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/gzread.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/gzwrite.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/infback.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/inffast.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/inflate.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/inftrees.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/trees.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/uncompr.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/zutil.c $(ZLIBHEADERS)>> $(LGS)
	@echo "Linking zlib..."
	@$(LINKERTOOL) rcs $(BINZLIB) $(OBJZLIB) >> $(LGS)
	@echo "Done!"
	@rm -rf *.o
	@echo "Compiling libpng..."
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/png.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngerror.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngget.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngmem.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngset.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngpread.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngread.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrio.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrtran.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrutil.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngtrans.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwio.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwrite.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwtran.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwutil.c $(PNGHEADERS)>> $(LGS)
	@echo "Linking libpng..."
	@$(LINKERTOOL) rcs $(BINLIBPNG) $(OBJLIBPNG) >> $(LGS)
	@echo "Done!"
	@rm -rf *.o

deps-w32: BINZLIB = ./lib/mingw-w32/zlib32.a
deps-w32: BINLIBPNG = ./lib/mingw-w32/lpng32.a
deps-w32: CARCH += -m32 -mwindows -static-libgcc
deps-w32: CDEFS += -DCOMPILER_MGW32
deps-w32: COMPILERPATH = C:/MinGW-w32
deps-w32: COMPILERPREFIX =
deps-w32: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
deps-w32: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
deps-w32: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
deps-w32: LINKERTOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)ar
deps-w32: build-info \
	make-deps

deps-w64: BINZLIB = ./lib/mingw-w64/zlib64.a
deps-w64: BINLIBPNG = ./lib/mingw-w64/lpng64.a
deps-w64: CARCH += -m64 -mwindows -static-libgcc
deps-w64: CDEFS += -DCOMPILER_MGW64
deps-w64: COMPILERPATH = C:/MinGW-w64
deps-w64: COMPILERPREFIX =
deps-w64: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
deps-w64: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
deps-w64: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
deps-w64: LINKERTOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)ar
deps-w64: build-info \
	make-deps

deps-tdm32: BINZLIB = ./lib/tdm-32/zlib32.a
deps-tdm32: BINLIBPNG = ./lib/tdm-32/lpng32.a
deps-tdm32: CARCH += -m32 -mwindows
deps-tdm32: CDEFS += -DCOMPILER_MGW32
deps-tdm32: COMPILERPATH = C:/MinGW-TDM32
deps-tdm32: COMPILERPREFIX =
deps-tdm32: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
deps-tdm32: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
deps-tdm32: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
deps-tdm32: LINKERTOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)ar
deps-tdm32: build-info \
	make-deps

deps-tdm64: BINZLIB = ./lib/tdm-64/zlib64.a
deps-tdm64: BINLIBPNG = ./lib/tdm-64/lpng64.a
deps-tdm64: CARCH += -m64 -mwindows
deps-tdm64: CDEFS += -DCOMPILER_MGW64
deps-tdm64: COMPILERPATH = C:/MinGW-TDM64
deps-tdm64: COMPILERPREFIX =
deps-tdm64: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
deps-tdm64: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
deps-tdm64: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
deps-tdm64: LINKERTOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)ar
deps-tdm64: build-info \
	make-deps

old-crap:
	@echo "This is an old, dead target!  Please don't use this!"
	@echo "Now Rebuilding zlib and libpng, 64-bit..."
	@echo "Compiling zlib..."
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/adler32.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/compress.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/crc32.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/deflate.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/gzclose.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/gzlib.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/gzread.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/gzwrite.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/infback.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/inffast.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/inflate.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/inftrees.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/trees.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/uncompr.c $(ZLIBHEADERS)>> $(LGS)
	@$(COMPILERC) $(ZLIBFLAGS) -O2 -w $(CINCLUDES) -c ./zlib/zutil.c $(ZLIBHEADERS)>> $(LGS)
	@echo "Linking zlib..."
	@$(LINKERTOOL) rcs $(BINZLIB) $(OBJZLIB) >> $(LGS)
	@echo "Done!"
	@rm -rf *.o
	@echo "Compiling libpng..."
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/png.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngerror.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngget.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngmem.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngset.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngpread.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngread.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrio.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrtran.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngrutil.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngtrans.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwio.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwrite.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwtran.c $(PNGHEADERS)>> $(LGS)
	@$(COMPILERC) $(PNGFLAGS) -O2 $(CINCLUDES) -c ./libpng/pngwutil.c $(PNGHEADERS)>> $(LGS)
	@echo "Linking libpng..."
	@$(LINKERTOOL) rcs $(BINLIBPNG) $(OBJLIBPNG) >> $(LGS)
	@echo "Done!"
	@rm -rf *.o

vst-w32: BINZLIB = ./lib/mingw-w32/zlib32.a
vst-w32: BINLIBPNG = ./lib/mingw-w32/lpng32.a
vst-w32: BINC120 = ./bin/mingw-w32/SC_32b_C120.dll
vst-w32: BINGLZ60 = ./bin/mingw-w32/SC_32b_GLZ60.dll
vst-w32: BINJYK = ./bin/mingw-w32/SC_32b_Jykwrakker.dll
vst-w32: BINGUITARIST = ./bin/mingw-w32/SC_32b_Guitarist.dll
vst-w32: CARCH += -m32 -mwindows $(STATICFLAGS)
vst-w32: CDEFS += -DCOMPILER_MGW32 -DBUILD_VST -DSC_ENABLE_STEREO_SOUND=1
vst-w32: COMPILERPATH = C:/MinGW-w32
vst-w32: COMPILERPREFIX =
vst-w32: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
vst-w32: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
vst-w32: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
vst-w32: GUILIBS = -L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB)
vst-w32: clear-logs \
	build-info \
	un-vstsdk \
	cm-vstgui \
	cm-c120-intfc \
	un-c120 \
	lkvst-c120 \
	cm-glz60-intfc \
	un-glz60 \
	lkvst-glz60 \
	cm-jyk-intfc \
	un-jyk \
	lkvst-jyk \
	cm-gtr-intfc \
	un-guitarist \
	lkvst-guitarist \
	clean-obj
	@echo -e "\rAll Done! \r" >> $(LGS)
	@echo -e "Built files are in the bin/ folder. " >> $(LGS)
	@echo " "
	@echo All Done!
	@echo Built files are in the bin/ folder.

vst-w64: BINZLIB = ./lib/mingw-w64/zlib64.a
vst-w64: BINLIBPNG = ./lib/mingw-w64/lpng64.a
vst-w64: BINC120 = ./bin/mingw-w64/SC_64b_C120.dll
vst-w64: BINGLZ60 = ./bin/mingw-w64/SC_64b_GLZ60.dll
vst-w64: BINJYK = ./bin/mingw-w64/SC_64b_Jykwrakker.dll
vst-w64: BINGUITARIST = ./bin/mingw-w64/SC_64b_Guitarist.dll
vst-w64: CARCH += -m64 -mwindows $(STATICFLAGS)
vst-w64: CDEFS += -DCOMPILER_MGW64 -DBUILD_VST -DSC_ENABLE_STEREO_SOUND=1
vst-w64: COMPILERPATH = C:/MinGW-w64
vst-w64: COMPILERPREFIX =
vst-w64: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
vst-w64: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
vst-w64: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
vst-w64: GUILIBS = -L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB)
vst-w64: clear-logs \
	build-info \
	un-vstsdk \
	cm-vstgui \
	cm-c120-intfc \
	un-c120 \
	lkvst-c120 \
	cm-glz60-intfc \
	un-glz60 \
	lkvst-glz60 \
	cm-jyk-intfc \
	un-jyk \
	lkvst-jyk \
	cm-gtr-intfc \
	un-guitarist \
	lkvst-guitarist \
	clean-obj
	@echo -e "\rAll Done! \r" >> $(LGS)
	@echo -e "Built files are in the bin/ folder. " >> $(LGS)
	@echo " "
	@echo All Done!
	@echo Built files are in the bin/ folder.

vst-tdm32: BINZLIB = ./lib/tdm-32/zlib32.a
vst-tdm32: BINLIBPNG = ./lib/tdm-32/lpng32.a
vst-tdm32: BINC120 = ./bin/tdm-32/SC_32b_C120.dll
vst-tdm32: BINGLZ60 = ./bin/tdm-32/SC_32b_GLZ60.dll
vst-tdm32: BINJYK = ./bin/tdm-32/SC_32b_Jykwrakker.dll
vst-tdm32: BINGUITARIST = ./bin/tdm-32/SC_32b_Guitarist.dll
vst-tdm32: CARCH += -m32 -mwindows
vst-tdm32: CDEFS += -DCOMPILER_MGW32 -DBUILD_VST -DSC_ENABLE_STEREO_SOUND=1
vst-tdm32: COMPILERPATH = C:/MinGW-TDM32
vst-tdm32: COMPILERPREFIX =
vst-tdm32: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
vst-tdm32: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
vst-tdm32: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
vst-tdm32: GUILIBS = -L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB)
vst-tdm32: clear-logs \
	build-info \
	un-vstsdk \
	cm-vstgui \
	cm-c120-intfc \
	un-c120 \
	lkvst-c120 \
	cm-glz60-intfc \
	un-glz60 \
	lkvst-glz60 \
	cm-jyk-intfc \
	un-jyk \
	lkvst-jyk \
	cm-gtr-intfc \
	un-guitarist \
	lkvst-guitarist \
	clean-obj
	@echo -e "\rAll Done! \r" >> $(LGS)
	@echo -e "Built files are in the bin/ folder. " >> $(LGS)
	@echo " "
	@echo All Done!
	@echo Built files are in the bin/ folder.

vst-tdm64: BINZLIB = ./lib/tdm-64/zlib64.a
vst-tdm64: BINLIBPNG = ./lib/tdm-64/lpng64.a
vst-tdm64: BINC120 = ./bin/tdm-64/SC_64b_C120.dll
vst-tdm64: BINGLZ60 = ./bin/tdm-64/SC_64b_GLZ60.dll
vst-tdm64: BINJYK = ./bin/tdm-64/SC_64b_Jykwrakker.dll
vst-tdm64: BINGUITARIST = ./bin/tdm-64/SC_64b_Guitarist.dll
vst-tdm64: BINDRUMMER = ./bin/tdm-64/SC_64b_Drummer.dll
vst-tdm64: CARCH += -m64 -mwindows
vst-tdm64: CDEFS += -DCOMPILER_MGW64 -DBUILD_VST -DSC_ENABLE_STEREO_SOUND=1
vst-tdm64: COMPILERPATH = C:/MinGW-TDM64
vst-tdm64: COMPILERPREFIX =
vst-tdm64: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
vst-tdm64: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
vst-tdm64: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
vst-tdm64: GUILIBS = -L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB)
vst-tdm64: clear-logs \
	build-info \
	un-vstsdk \
	cm-vstgui \
	cm-c120-intfc \
	un-c120 \
	lkvst-c120 \
	cm-glz60-intfc \
	un-glz60 \
	lkvst-glz60 \
	cm-jyk-intfc \
	un-jyk \
	lkvst-jyk \
	cm-gtr-intfc \
	un-guitarist \
	lkvst-guitarist \
	clean-obj
	@echo -e "\rAll Done! \r" >> $(LGS)
	@echo -e "Built files are in the bin/ folder. " >> $(LGS)
	@echo " "
	@echo All Done!
	@echo Built files are in the bin/ folder.

lkvst-c120:
	@echo -e "Creating Resource File... \r" >> $(LGS)
	@echo Creating Resource File...
	@$(RESOURCETOOL) -i ./vst_c120/c120.rc -J rc -o rc_c120.o -O coff >> $(LGS)
	@echo -e "Linking C120 VST... \r" >> $(LGS)
	@echo Linking C120 VST...
	@$(COMPILERCPP) -shared -Wl,--dll,-l:$(BINLIBPNG),-l:$(BINZLIB) -s $(LIBS) \
	-L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB) \
	$(OBJVSTGUI) $(OBJVSTSDK) $(OBJVSTC120) rc_c120.o \
	-o $(BINC120) $(BINLIBPNG) $(BINZLIB) --def ./vst_c120/c120.def $(CPPFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) -l:$(BINLIBPNG) -l:$(BINZLIB) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

lkvst-glz60:
	@echo -e "Creating Resource File... \r" >> $(LGS)
	@echo Creating Resource File...
	@$(RESOURCETOOL) -i ./vst_glz60/glz60.rc -J rc -o rc_glz60.o -O coff >> $(LGS)
	@echo -e "Linking GLZ60 VST... \r" >> $(LGS)
	@echo Linking GLZ60 VST...
	@$(COMPILERCPP) -shared -Wl,--dll -s $(LIBS) \
	-L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB) \
	$(OBJVSTGUI) $(OBJVSTSDK) $(OBJVSTGLZ60) rc_c120.o \
	-o $(BINGLZ60) $(BINLIBPNG) $(BINZLIB) --def ./vst_glz60/glz60.def $(CPPFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) -l:$(BINLIBPNG) -l:$(BINZLIB) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

lkvst-jyk:
	@echo -e "Creating Resource File... \r" >> $(LGS)
	@echo Creating Resource File...
	@$(RESOURCETOOL) -i ./vst_jykkr/jyk.rc -J rc -o rc_jyk.o -O coff >> $(LGS)
	@echo -e "Linking Jyk VST... \r" >> $(LGS)
	@echo Linking Jyk VST...
	@$(COMPILERCPP) -shared -Wl,--dll -s $(LIBS) \
	-L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB) \
	$(OBJVSTGUI) $(OBJVSTSDK) $(OBJVSTJYK) rc_jyk.o \
	-o $(BINJYK) $(BINLIBPNG) $(BINZLIB) --def ./vst_jyk/jyk.def $(CPPFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) -l:$(BINLIBPNG) -l:$(BINZLIB) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

lkvst-guitarist:
	@echo -e "Linking Guitarist VSTi... \r" >> $(LGS)
	@echo Linking Guitarist VSTi...
	@$(COMPILERCPP) -shared -Wl,--dll -s $(LIBS) \
	-L./ -L./lib \
	$(OBJVSTGUITARIST) $(OBJVSTSDK) \
	-o $(BINGUITARIST) --def ./vsti_guitarist/guitarist.def $(CPPFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

lkvst-drummer:
	@echo -e "Linking Drummer VSTi... \r" >> $(LGS)
	@echo Linking Drummer VSTi...
	@$(COMPILERCPP) -shared -Wl,--dll -s $(LIBS) \
	-L./ -L./lib \
	$(OBJVSTDRUMMER) $(OBJVSTSDK) \
	-o $(BINDRUMMER) --def ./vsti_drummer/drummer.def $(CPPFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

peakwriter-w32: BINPKWRT = ./bin/mingw-w32/SC_pwrite32.exe
peakwriter-w32: CARCH += -m32 -mwindows $(STATICFLAGS)
peakwriter-w32: CDEFS += -DCOMPILER_MGW32 -DBUILD_VST -DSC_ENABLE_STEREO_SOUND=1
peakwriter-w32: COMPILERPATH = C:/MinGW-w32
peakwriter-w32: COMPILERPREFIX =
peakwriter-w32: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
peakwriter-w32: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
peakwriter-w32: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
peakwriter-w32: clear-logs \
	build-info
	@echo "Building PeakWriter Utility, 32-bits..."
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O1 $(CINCLUDES) \
	./peakwriter/peakwriter.cpp ./peakwriter/reader_wav.cpp -o $(BINPKWRT)
	@echo "Done!"

cm-suite-intfc:
	@echo -e "Building Suite VST Interface code \r" >> $(LGS)
	@echo Building Suite VST Interface code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./vst_suite/suite.cpp >> $(LGS)
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -I./common -c ./vst_suite/suite_gui.cpp >> $(LGS)

un-suite:
	@echo -e "Suite VST Plugin, actual code \r">> $(LGS)
	@echo Suite VST Plugin, actual code
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O2 $(CINCLUDES) -c ./common_c/un_vst_suite.c >> $(LGS)

lkvst-suite:
	@echo -e "Creating Resource File... \r" >> $(LGS)
	@echo Creating Resource File...
	@$(RESOURCETOOL) -i ./vst_suite/suite.rc -J rc -o rc_suite.o -O coff >> $(LGS)
	@echo -e "Linking SC VST Suite... \r" >> $(LGS)
	@echo Linking SC VST Suite...
	@$(COMPILERCPP) -shared -Wl,--dll,-l:$(BINLIBPNG),-l:$(BINZLIB) -s $(LIBS) \
	-L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB) \
	$(OBJVSTSUITE) $(OBJVSTGUI) $(OBJVSTSDK) rc_suite.o \
	-o $(BINSUITE) $(BINLIBPNG) $(BINZLIB) --def ./vst_suite/suite.def $(CPPFLAGS) $(ADDCPPFLAGS) \
	$(CARCH) $(CDEFS) $(LIBS) -l:$(BINLIBPNG) -l:$(BINZLIB) >> $(LGS)
	@echo -e "Done! \r" >> $(LGS)
	@echo Done!

vst-suite-w32: BINZLIB = ./lib/mingw-w32/zlib32.a
vst-suite-w32: BINLIBPNG = ./lib/mingw-w32/lpng32.a
vst-suite-w32: BINSUITE = ./bin/mingw-w32/SC_Suite_32.dll
vst-suite-w32: CARCH += -m32 -mwindows $(STATICFLAGS)
vst-suite-w32: CDEFS += -DCOMPILER_MGW32 -DBUILD_VST -DSC_ENABLE_STEREO_SOUND=1 -DSCr_GUI_Num_Visible_Slots=$(SLOTS)
vst-suite-w32: COMPILERPATH = C:/MinGW-w32
vst-suite-w32: COMPILERPREFIX =
vst-suite-w32: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
vst-suite-w32: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
vst-suite-w32: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
vst-suite-w32: GUILIBS = -L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB)
vst-suite-w32: clear-logs \
	build-info \
	un-vstsdk \
	cm-vstgui \
	cm-suite-intfc \
	un-suite \
	lkvst-suite \
	clean-obj
	@echo -e "\rAll Done! \r" >> $(LGS)
	@echo -e "Built files are in the bin/ folder. " >> $(LGS)
	@echo " "
	@echo All Done!
	@echo Built files are in the bin/ folder.

vst-suite-w64: BINZLIB = ./lib/mingw-w64/zlib64.a
vst-suite-w64: BINLIBPNG = ./lib/mingw-w64/lpng64.a
vst-suite-w64: BINSUITE = ./bin/mingw-w64/SC_Suite_64.dll
vst-suite-w64: CARCH += -m64 -mwindows $(STATICFLAGS)
vst-suite-w64: CDEFS += -DCOMPILER_MGW64 -DBUILD_VST -DSC_ENABLE_STEREO_SOUND=1 -DSCr_GUI_Num_Visible_Slots=$(SLOTS)
vst-suite-w64: COMPILERPATH = C:/MinGW-w64
vst-suite-w64: COMPILERPREFIX =
vst-suite-w64: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
vst-suite-w64: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
vst-suite-w64: RESOURCETOOL = $(COMPILERPATH)/bin/$(COMPILERPREFIX)windres
vst-suite-w64: GUILIBS = -L./ -L./lib -l:$(BINLIBPNG) -l:$(BINZLIB)
vst-suite-w64: clear-logs \
	build-info \
	un-vstsdk \
	cm-vstgui \
	cm-suite-intfc \
	un-suite \
	lkvst-suite \
	clean-obj
	@echo -e "\rAll Done! \r" >> $(LGS)
	@echo -e "Built files are in the bin/ folder. " >> $(LGS)
	@echo " "
	@echo All Done!
	@echo Built files are in the bin/ folder.

cm-firdata:
	@echo "Building FIR Data Utility, 64-bits..."
	@$(COMPILERCPP) $(BLDCPP) $(CARCH) $(CDEFS) -O0 $(CINCLUDES) \
	./fir_data_converter/firdata.cpp -o $(BINTOOLPATH)

firdata-tdm64: CARCH += -m64 -mwindows
firdata-tdm64: COMPILERPATH = C:/MinGW-TDM64
firdata-tdm64: COMPILERC = $(COMPILERPATH)/bin/$(COMPILERPREFIX)gcc
firdata-tdm64: COMPILERCPP = $(COMPILERPATH)/bin/$(COMPILERPREFIX)g++
firdata-tdm64: BINTOOLPATH = ./bin/tdm-64/firdata.exe
firdata-tdm64: clear-logs \
	build-info \
	cm-firdata \
	clean-obj
	@echo "Done!"
