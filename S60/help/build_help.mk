# ============================================================================
#  Name	 : build_help.mk
#  Part of  : DemoVideoCall
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : DemoVideoCall
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE : DemoVideoCall_0xE5EC06EB.hlp
DemoVideoCall_0xE5EC06EB.hlp : DemoVideoCall.xml DemoVideoCall.cshlp Custom.xml
	cshlpcmp DemoVideoCall.cshlp
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
	copy DemoVideoCall_0xE5EC06EB.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del DemoVideoCall_0xE5EC06EB.hlp
	del DemoVideoCall_0xE5EC06EB.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo DemoVideoCall_0xE5EC06EB.hlp

FINAL : do_nothing
