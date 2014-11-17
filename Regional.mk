#
# Some Makefile includes specific to the regional maps
#

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
ERRGRD = $(wildcard ../global_vs30*_$(MYEXT)*.grd)
ifneq ($(strip $(ERRGRD)),)
# If the directory above already has my extension, don't add it again
OUTGRD = $(notdir $(INGRD))
else
# This is the normal case
OUTGRD = $(basename $(notdir $(INGRD)))_$(MYEXT).grd
endif

#
# Generate a warning if this region's extension is
# already in the global Vs30 file. For makes other
# than "all", just touch the grid so it is newer
# than its dependencies. Otherwise, set up the
# normal targets:
#
ERRGRD = $(wildcard ../global_vs30*_$(MYEXT)*.grd)

ifneq ($(strip $(ERRGRD)),)

all :
	$(warning Skipping $(MYNAME) processing: The global Vs30 directory\
	          $Nalready contains a global grid file with the '_$(MYEXT)' regional stamp.\
	          $NIf that's not what you want, you'll need to remove '$(ERRGRD)'\
	          $Nor replace it with a global Vs30 file that doesn't already have\
	          $Nthe '_$(MYEXT)' stamp)

../$(OUTGRD) :
	touch -c $@

else

all : ../$(OUTGRD)

../$(OUTGRD) : $(OUTGRD)
	$(RM) $(INGRD)
	cp $< ..

endif

#################################
#
# This is the fallback if we don't find ../global_vs30*.grd.
#
../global_vs30.grd : ../Slope/global_vs30.grd
	cp $< $@

../Slope/global_vs30.grd :
	$(MAKE) -C ../Slope global_vs30.grd

#
# These make sure the C programs are up to date
#
../src/smooth :
	$(MAKE) -C ../src smooth

../src/insert_grd :
	$(MAKE) -C ../src insert_grd


# End
