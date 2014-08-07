#
# Makefile for PNW Vs30 grid into the global+CA grid
#
include ../Constants.mk

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

#
# Pick one or the other map to insert.
# According to Eric Thompson:
# Taiwan_Vs30_AW09gk_30c.grd uses a baseline model from topographic slope
# (Allen & Wald, 2009) and is then adjusted for the mean residual in each 
# geologic unit and then the residuals are kriged.
# Taiwan_Vs30_PRG_30c.grd uses a 'hybrid trend' with a slope that is allowed
# to vary by geologic unit. This model is closest to the later work on 
# California and elsewhere.
#
#TW_GRD_FILE = Taiwan_Vs30_AW09gk_30c.grd
TW_GRD_FILE = Taiwan_Vs30_PRG_30c.grd

.PHONY: all plots clean clean_plots veryclean

#
# Regional.mk has the target "all" in it:
#
# all : ../$(OUTGRD)
#

../$(OUTGRD) : $(OUTGRD)
	$(RM) $(INGRD)
	cp $< ..

plots :  plot_raw_map plot_weights plot_final_map

clean : clean_plots
	$(RM) taiwan.grd weights.grd 

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
$(OUTGRD) : $(INGRD) taiwan.grd weights.grd ../src/insert_grd \
            insert_tw.par
	../src/insert_grd par=insert_tw.par grid1=$(INGRD) gout=$(OUTGRD)

#################################
#
# This is the fallback if we don't find ../global_vs30*.grd.
#
../global_vs30.grd : ../Slope/global_vs30.grd
	cp $< $@

../Slope/global_vs30.grd :
	$(MAKE) -C ../Slope global_vs30.grd

#
# Make a weighted clipping mask; this is trivial in this case
# because our region is entirely surrounded by water; the 
# weights = 1 where we have Vs30, = 0 where we don't 
#
weights.grd : $(TW_GRD_FILE)
	grdmath $< 0 AND = $@

#
# Make the insert map, set NaNs to the water velocity
#
taiwan.grd : $(TW_GRD_FILE)
	grdmath $< $(WATER) AND = $@

$(TW_GRD_FILE) : 
	( echo "File $(TW_GRD_FILE) must be supplied." && false )

#
# 
#
../src/insert_grd :
	$(MAKE) -C ../src insert_grd


plot_weights : weights.png

plot_final_map : taiwan.png

plot_raw_map : taiwan.png

# Plot the weights:
 
weights.png : weights.grd
	grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba4dg2d/a2eg2dWSen -K > weights.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps
	pscoast -JM12 -R$(TW_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 -density 300x300 weights.ps weights.png
 
# Plot the new Vs30 map:
 
taiwan.png : taiwan.grd
	grdimage $< -JM12 -C$(VS30_CPT) -Ba6d/a4eWSen -K > taiwan_region.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> taiwan_region.ps
	pscoast -JM12 -R$(TW_REGION) -O -Df -N1 -N2 -W >> taiwan_region.ps
	convert -trim -density 300x300 -rotate 90 taiwan_region.ps taiwan_region.png

taiwan_region.grd : global_vs30*_$(MYEXT).grd
	grdcut -R$(TW_REGION) $< -G$@

# Plot the raw geology map using its own scale.

taiwan_raw.png : taiwan.grd
	grdimage $< -JM12 -C$(VS30_CPT) -Ba4d/a2eWSen -K > taiwan_raw.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O >> taiwan_raw.ps
	convert -trim -rotate 90 -density 300x300 taiwan_raw.ps taiwan_raw.png

