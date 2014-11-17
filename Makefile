#
# Top-level Makefile for the Vs30 project
#

#
# Edit the list below to include only the insert maps you want 
# in the final product. "Slope" should always be the first
# item in the list. Add new map directories here, and in 
# MKDIRS, below.
#
INSERT_MAPS = Slope \
              California \
              PNW \
              Utah \
			  Japan \
			  Taiwan

#
# This should be a complete list of subdirectories so that
# the clean and veryclean targets can do their thing. Add
# new map directories here, and in INSERT_MAPS, above.
#
MKDIRS = src \
         Slope \
         California \
         PNW \
         Utah \
		 Japan \
		 Taiwan

MKDIRS_CLEAN = $(patsubst %,%.clean,$(MKDIRS))
MKDIRS_VCLEAN = $(patsubst %,%.vclean,$(MKDIRS))

.PHONY: all slope clean veryclean $(INSERT_MAPS) $(MKDIRS_CLEAN) $(MKDIRS_VCLEAN)

all : $(INSERT_MAPS)

clean : $(MKDIRS_CLEAN)

veryclean : $(MKDIRS_VCLEAN)

spotless : veryclean
	$(RM) global_vs30*.grd

$(INSERT_MAPS) :
	$(MAKE) -C $@

$(MKDIRS_CLEAN) :
	$(MAKE) -C $(@:.clean=) clean

$(MKDIRS_VCLEAN) :
	$(MAKE) -C $(@:.vclean=) veryclean

