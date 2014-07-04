#
# Some Makefile includes specific to the regional maps
#

# Don't mess with this definition -- the blank lines are necessary
define N


endef

#
# Identify the target into which we will insert the regional maps.
# If the target isn't there, set it to the base target "global_vs30.grd"
#
INGRD = $(wildcard ../global_vs30*.grd)
ifeq ($(strip $(INGRD)),)
        INGRD = ../global_vs30.grd
endif

#
# Add this region's extension to the base global file name
# to create the output file name
#
OUTGRD = $(basename $(notdir $(INGRD)))_$(MYEXT).grd

#
# Generate an error and quit if this region's extension is
# already in the global Vs30 file.
#
ERRGRD = $(wildcard ../global_vs30*_$(MYEXT)*.grd)

ifneq ($(strip $(ERRGRD)),)

all :
	$(warning Skipping $(MYNAME) processing: The global Vs30 directory\
                $Nalready contains a global grid file with the '_$(MYEXT)' regional stamp.\
                $NIf that's not what you want, you'll need to remove '$(ERRGRD)'\
                $Nor replace it with a global Vs30 file that doesn't already have\
                $Nthe '_$(MYEXT)' stamp)
else

all : ../$(OUTGRD)

endif

# End
