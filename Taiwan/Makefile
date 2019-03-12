#
# Makefile for Taiwan Vs30 grid into the global+regional grid
#

include ~/.vs30/Constants.mk

TW_XMIN = 119.0
TW_XMAX = 123.0
TW_YMIN = 21.0
TW_YMAX = 26.0
TW_REGION = $(TW_XMIN)/$(TW_XMAX)/$(TW_YMIN)/$(TW_YMAX)

IS_FINER := $(shell [ $(IRES) -lt 30 ] && echo true)

.PHONY: all plots clean clean_plots veryclean

all ::taiwan.grd weights.grd

plots :  plot_raw_map plot_weights plot_final_map

clean : clean_plots
	$(RM) taiwan.grd weights.grd tw_*c.grd gmt.history

clean_plots :
	$(RM) *.ps *.png
	$(RM) taiwan_region.grd

veryclean : clean
	$(RM) global_vs30*_$(MYEXT).grd

###############################################################
# Make a weighted clipping mask; this is trivial in this case
# because our region is entirely surrounded by water; the 
# weights = 1 where we have Vs30, = 0 where we don't 
#

weights.grd : tw_$(RES)c.grd
	gmt grdmath $< 0 AND 0 GT = $@

###############################################################
# Make the insert map, set NaNs to the water velocity
#

taiwan.grd : tw_$(RES)c.grd
	gmt grdmath $< $(WATER) AND = $@

ifeq ($(IS_FINER),true)
tw_$(RES)c.grd : $(TW_GRD_FILE)
	gmt grdsample -nl+t0.1 -I$(RES)s -fg $< -G$@
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

# Set some flags for plotting. To change things for an individual map,
# make sure to only change it in that particular code block below.

Jflags = M12		# Mercator projection, 12 cm across
Bflags = a2f1WSen	# Tick marks. Every 2 degrees deneoted with number, 1 degree tickmarks, print numbers on W and S axes
Dflags = 14/4.3/9/0.5	# Scalebar - position 14, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -J$(Jflags) -B$(Bflags) -K > weights.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps
	gmt pscoast -J$(Jflags) -R$(TW_REGION) -Df -O -N1 -N2 -W >> weights.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) weights.ps
 
# Plot the new Vs30 map:
ONE = 1.0
TW_REG_XMIN = $(shell echo $(TW_XMIN) - $(ONE) | bc)
TW_REG_XMAX = $(shell echo $(TW_XMAX) + $(ONE) | bc)
TW_REG_YMIN = $(shell echo $(TW_YMIN) - $(ONE) | bc)
TW_REG_YMAX = $(shell echo $(TW_YMAX) + $(ONE) | bc)
TW_REG = $(TW_REG_XMIN)/$(TW_REG_XMAX)/$(TW_REG_YMIN)/$(TW_REG_YMAX)
 
taiwan.png : taiwan_region.grd
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > taiwan_region.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> taiwan_region.ps
	gmt pscoast -J$(Jflags) -R$(TW_REG) -O -Df -N1 -N2 -W >> taiwan_region.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) taiwan_region.ps
	rm gmt.history

taiwan_region.grd : ../global_vs30.grd
	gmt grdcut -R$(TW_REG) $< -G$@

# Plot the raw geology map

taiwan_raw.png : $(TW_GRD_FILE)
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > taiwan_raw.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> taiwan_raw.ps
	gmt pscoast -J$(Jflags) -R$(TW_REGION) -O -Df -N1 -N2 -W >> taiwan_raw.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) taiwan_raw.ps
