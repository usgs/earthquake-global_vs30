#
# Makefile for Taiwan Vs30 grid into the global+regional grid
#
include ~/.vs30/Constants.mk

#
# MYEXT is this region's insert map extension. MYNAME is just for
# informational messages. MYEXT must be defined before the Regional.mk
# include file.
#
MYEXT = tw
MYNAME = Taiwan

#
# Regional include; sets INGRD and OUTGRD.
#
include ../Regional.mk

TW_XMIN = 119.0
TW_XMAX = 123.0
TW_YMIN = 21.0
TW_YMAX = 26.0
TW_REGION = $(TW_XMIN)/$(TW_XMAX)/$(TW_YMIN)/$(TW_YMAX)

IS_FINER := $(shell [ $(IRES) -lt 30 ] && echo true)

.PHONY: all plots clean clean_plots veryclean

#
# Regional.mk has the targets "all" and ../$(OUTGRD) in it:
#
# all : ../$(OUTGRD)
#
#
#../$(OUTGRD) : $(OUTGRD)
#	$(RM) $(INGRD)
#	cp $< ..
#

plots :  plot_raw_map plot_weights plot_final_map

clean : clean_plots
	$(RM) taiwan.grd weights.grd tw_*c.grd

clean_plots :
	$(RM) *.ps *.png
	$(RM) taiwan_region.grd

veryclean : clean
	$(RM) global_vs30*_$(MYEXT).grd

#################################
#
# Run the insert program; this will insert the Japan map into the 
# global grid that may already have other maps inserted:
#
$(OUTGRD) : $(INGRD) taiwan.grd weights.grd ../src/insert_grd 
	../src/insert_grd grid1=$(INGRD) grid2=taiwan.grd gmask=weights.grd gout=$@

#
# Make a weighted clipping mask; this is trivial in this case
# because our region is entirely surrounded by water; the 
# weights = 1 where we have Vs30, = 0 where we don't 
#
weights.grd : tw_$(RES)c.grd
	grdmath $< 0 AND 0 GT = $@

#
# Make the insert map, set NaNs to the water velocity
#
taiwan.grd : tw_$(RES)c.grd
	grdmath $< $(WATER) AND = $@

ifeq ($(IS_FINER),true)
tw_$(RES)c.grd : $(TW_GRD_FILE)
	grdsample -Ql0.1 -I$(RES)c -fg $< -G$@
else
tw_$(RES)c.grd : $(TW_GRD_FILE)
	cp $< $@
endif

$(TW_GRD_FILE) : 
	( echo "File $(TW_GRD_FILE) must be supplied." && false )

####################################
#
# Plots
#
plot_weights : weights.png

plot_final_map : taiwan.png

plot_raw_map : taiwan_raw.png

# Plot the weights:
 
weights.png : weights.grd
	grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba4dg2d/a2eg2dWSen -K > weights.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps
	pscoast -JM12 -R$(TW_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 -density 300x300 weights.ps weights.png
 
# Plot the new Vs30 map:
ONE = 1.0
TW_REG_XMIN = $(shell echo $(TW_XMIN) - $(ONE) | bc)
TW_REG_XMAX = $(shell echo $(TW_XMAX) + $(ONE) | bc)
TW_REG_YMIN = $(shell echo $(TW_YMIN) - $(ONE) | bc)
TW_REG_YMAX = $(shell echo $(TW_YMAX) + $(ONE) | bc)
TW_REG = $(TW_REG_XMIN)/$(TW_REG_XMAX)/$(TW_REG_YMIN)/$(TW_REG_YMAX)
 
taiwan.png : taiwan_region.grd
	grdimage $< -JM12 -C$(VS30_CPT) -Ba6d/a4eWSen -K > taiwan_region.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> taiwan_region.ps
	pscoast -JM12 -R$(TW_REG) -O -Df -N1 -N2 -W >> taiwan_region.ps
	convert -trim -density 300x300 -rotate 90 taiwan_region.ps taiwan_region.png

taiwan_region.grd : ../$(OUTGRD)
	grdcut -R$(TW_REG) $< -G$@

# Plot the raw geology map

taiwan_raw.png : $(TW_GRD_FILE)
	grdimage $< -JM12 -C$(VS30_CPT) -Ba4d/a2eWSen -K > taiwan_raw.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> taiwan_raw.ps
	pscoast -JM12 -R$(TW_REGION) -O -Df -N1 -N2 -W >> taiwan_raw.ps
	convert -trim -rotate 90 -density 300x300 taiwan_raw.ps taiwan_raw.png

