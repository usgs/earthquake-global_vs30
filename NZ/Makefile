#
# Makefile for NZ Vs30 grid into the global+regional grid
#
include ../Constants.mk

#
# MYEXT is this region's insert map extension. MYNAME is just for
# informational messages. MYEXT must be defined before the Regional.mk
# include file.
#
MYEXT = nz
MYNAME = NewZealand

#
# Regional include; sets INGRD and OUTGRD.
#
include ../Regional.mk

NZ_XMIN = 166.0
NZ_XMAX = 179.0
NZ_YMIN = -47.5
NZ_YMAX = -34.0
NZ_REGION = $(NZ_XMIN)/$(NZ_XMAX)/$(NZ_YMIN)/$(NZ_YMAX)

IS_FINER := $(shell [ $(IRES) -lt 36 ] && echo true)

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
	$(RM) newzealand.grd weights.grd nz_*c.grd vs30_nz_gmt4.grd 

clean_plots :
	$(RM) *.ps *.png
	$(RM) newzealand_region.grd

veryclean : clean
	$(RM) global_vs30*_$(MYEXT).grd

#################################
#
# Run the insert program; this will insert the NZ map into the 
# global grid that may already have other maps inserted:
#
$(OUTGRD) : $(INGRD) newzealand.grd weights.grd ../src/insert_grd 
	../src/insert_grd grid1=$(INGRD) grid2=newzealand.grd gmask=weights.grd gout=$@

#
# Make a weighted clipping mask; this is trivial in this case
# because our region is entirely surrounded by water; the 
# weights = 1 everywhere
#
weights.grd : newzealand.grd
	grdmath $< 0 MUL 1 ADD = $@

#
# Make the insert map, Set values < 50 to 0, between 50 and 90 to 90,
# and then set zeros to the water velocity
#
newzealand.grd : nz_$(RES)c.grd
	grdmath $< DUP 50 GT MUL DUP DUP 0 GT EXCH 90 LE MUL 90 MUL EXCH DUP 90 GT MUL ADD DUP 0 EQ $(WATER) MUL ADD = $@

ifeq ($(IS_FINER),true)
nz_$(RES)c.grd : vs30_nz_gmt4.grd
	grdsample -Ql0.1 -I$(RES)c -fg $< -G$@
else
nz_$(RES)c.grd : vs30_nz_gmt4.grd
	cp $< $@
endif

vs30_nz_gmt4.grd : vs30_nz_gmt4_pixel.grd
	grdsample $< -G$@ -T

vs30_nz_gmt4_pixel.grd : 
	( echo "File vs30_nz_gmt4_pixel.grd must be supplied." && false )

####################################
#
# Plots
#
plot_weights : weights.png

plot_final_map : newzealand.png

plot_raw_map : newzealand_raw.png

# Plot the weights:
 
weights.png : weights.grd
	grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba4dg2d/a2eg2dWSen -K > weights.ps
	psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps
	pscoast -JM12 -R$(NZ_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 -density 300x300 weights.ps weights.png
 
# Plot the new Vs30 map:
ONE = 1.0
NZ_REG_XMIN = $(shell echo $(NZ_XMIN) - $(ONE) | bc)
NZ_REG_XMAX = $(shell echo $(NZ_XMAX) + $(ONE) | bc)
NZ_REG_YMIN = $(shell echo $(NZ_YMIN) - $(ONE) | bc)
NZ_REG_YMAX = $(shell echo $(NZ_YMAX) + $(ONE) | bc)
NZ_REG = $(NZ_REG_XMIN)/$(NZ_REG_XMAX)/$(NZ_REG_YMIN)/$(NZ_REG_YMAX)
 
newzealand.png : newzealand_region.grd
	grdimage $< -JM12 -C$(VS30_CPT) -Ba6d/a4eWSen -K > newzealand_region.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> newzealand_region.ps
	pscoast -JM12 -R$(NZ_REG) -O -Df -N1 -N2 -W >> newzealand_region.ps
	convert -trim -density 300x300 -rotate 90 newzealand_region.ps newzealand_region.png

newzealand_region.grd : ../$(OUTGRD)
	grdcut -R$(NZ_REG) $< -G$@

# Plot the raw geology map

newzealand_raw.png : vs30_nz_gmt4.grd
	grdimage $< -JM12 -C$(VS30_CPT) -Ba4d/a2eWSen -K > newzealand_raw.ps
	psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> newzealand_raw.ps
	pscoast -JM12 -R$(NZ_REGION) -O -Df -N1 -N2 -W >> newzealand_raw.ps
	convert -trim -rotate 90 -density 300x300 newzealand_raw.ps newzealand_raw.png

