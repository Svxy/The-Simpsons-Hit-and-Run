################################################################################
#
#  Definition:	This file builds the libraries that aren't fully RadBuild-compliant
#               and adds the generated files to the defined LIBS 
################################################################################

ifeq ($(CONFIG), DEBUG)
P3DLIB = debug
P3DCLEANEXT = d
P3DOLDLIBEXT = pmD
P3DNEWLIBEXT = md
endif

ifeq ($(CONFIG), RELEASE)
P3DLIB = release
P3DCLEANEXT = r
P3DOLDLIBEXT = pmR
P3DNEWLIBEXT = mr
endif

ifeq ($(CONFIG), TUNE)
P3DLIB = tune
P3DCLEANEXT = t
P3DOLDLIBEXT = pmT
P3DNEWLIBEXT = mt
endif

INCLUDEDIR += 	$(ROOTDIR)/radmath \
		$(ROOTDIR)/pure3d \
		$(ROOTDIR)/pure3d/p3d \
		$(ROOTDIR)/pure3d/pddi \
		$(ROOTDIR)/poser \
		$(ROOTDIR)/poser/inc \
		$(ROOTDIR)/sim 


LIBS += $(ROOTDIR)/sim/lib/sim$(P3DOLDLIBEXT).a \
	$(ROOTDIR)/pure3d/build/lib/pure3d$(P3DNEWLIBEXT).a \
        $(ROOTDIR)/pure3d/build/lib/pddi$(P3DNEWLIBEXT).a \
	$(ROOTDIR)/pure3d/build/lib/png$(P3DNEWLIBEXT).a \
	$(ROOTDIR)/pure3d/build/lib/zlib$(P3DNEWLIBEXT).a \
	$(ROOTDIR)/poser/lib/poser$(P3DOLDLIBEXT).a \
	$(ROOTDIR)/radmath/lib/radmath$(P3DNEWLIBEXT).a \
	/usr/local/sce/ee/gcc/ee/lib/libstdc++.a

libs : non_ftt_libs

libsclean : non_ftt_libs_clean

non_ftt_libs :
	@echo -----------------------------
	@echo Updating pure3d libraries ...
	@echo -----------------------------
	@$(ROOTDIR)\\radbuild\\globaltools\\make --no-print-directory -r -C $(ROOTDIR)/pure3d/build/ps2mw LIB=$(P3DLIB)
	@echo -----------------------------
	@echo Updating radmath library ...
	@echo -----------------------------
	@$(ROOTDIR)\\radbuild\\globaltools\\make --no-print-directory -r -C $(ROOTDIR)/radmath/build/ps2mw LIB=$(P3DLIB)
	@echo -----------------------------
	@echo Updating Sim Library ...
	@echo -----------------------------
	@$(ROOTDIR)\\radbuild\\globaltools\\make --no-print-directory -r -C $(ROOTDIR)/sim/build/ps2mw $(P3DLIB)
	@echo -----------------------------
	@echo Updating Poser Library ...
	@echo -----------------------------
	@$(ROOTDIR)\\radbuild\\globaltools\\make --no-print-directory -r -C $(ROOTDIR)/poser/build/ps2mw $(P3DLIB)

non_ftt_libs_clean :
	@echo -----------------------------
	@echo Cleaning pure3d libraries ...
	@echo -----------------------------
	@$(ROOTDIR)\\radbuild\\globaltools\\make --no-print-directory -r -C $(ROOTDIR)/pure3d/build/ps2mw clean LIB=$(P3DCLEANEXT)
	@echo -----------------------------
	@echo Cleaning radmath library ...
	@echo -----------------------------
	@$(ROOTDIR)\\radbuild\\globaltools\\make --no-print-directory -r -C $(ROOTDIR)/radmath/build/ps2mw LIB=clean$(P3DCLEANEXT)
	@echo -----------------------------
	@echo Cleaning Sim library ...
	@echo -----------------------------
	@$(ROOTDIR)\\radbuild\\globaltools\\make --no-print-directory -r -C $(ROOTDIR)/sim/build/ps2mw LIB=clean$(P3DCLEANEXT)
	@echo -----------------------------
	@echo Cleaning Poser library ...
	@echo -----------------------------
	@$(ROOTDIR)\\radbuild\\globaltools\\make --no-print-directory -r -C $(ROOTDIR)/poser/build/ps2mw LIB=clean$(P3DCLEANEXT)
