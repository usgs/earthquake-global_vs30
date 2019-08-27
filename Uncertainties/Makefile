# This Makefile creates the topographic slope-based uncertainty map
# and overlays the regional uncertainty maps - where available - over
# it. So far we have California and Greece.

include ~/.vs30/Constants.mk

GLOBAL_REGION = -180/180/-56/84
SMALLER_REGION = -128/-100/30/46

CA_WEST = -124.5
CA_EAST = -114.1
CA_NORTH = 42.1
CA_SOUTH = 32.5

CALI_REGION = $(CA_WEST)/$(CA_EAST)/$(CA_SOUTH)/$(CA_NORTH)

ZEROS_W = -180/$(CA_WEST)/-56/84
ZEROS_E = $(CA_EAST)/180/-56/84
ZEROS_N = $(CA_WEST)/$(CA_EAST)/$(CA_NORTH)/84
ZEROS_S = $(CA_WEST)/$(CA_EAST)/-56/$(CA_SOUTH)

GREECE_REGION = 16.5/31.5/34/43

IS_COARSER := $(shell [ $(IRES) -gt 7 ] && echo true)

all :  vs30_slope_uncert.grd global_uncert_ca_gr.grd

plots : global_uncert_ca vs30_slope_uncert cali_sd global_uncert_ca_gr

clean : clean_plots
	$(RM) vs30_slope_uncert.grd global_uncert_ca*.grd landmask_land.grd cali_weight.grd \
        greece_uncert.grd greece_full_uncert.grd gr_weights.grd north.grd south.grd east.grd west.grd \
        cali_sd* gmt.history

clean_plots : 
	$(RM) *.ps *.png *.pdf

veryclean : clean

##################################################################
# This time I used insert_grd to insert new Greece map - worked 
# without a problem. The California border problem must be
# specific to that workflow.

global_uncert_ca_gr.grd : ../src/insert_grd global_uncert_ca.grd greece_full_uncert.grd gr_weights.grd
	../src/insert_grd gin=global_uncert_ca.grd gout=$@ grid1=greece_full_uncert.grd gmask1=gr_weights.grd

##################################################################
# Now for the Greece map.
#
# Using the site-specific Vs30 uncertainty values, first convert 
# the .xy file into a grd, then make the rest of Greece 0.4, as 
# per Stewart et al., 2014. The .xy file was created using the
# uncert. criteria listed in the publication, along with the ~300 
# site-specific measurements in the electronic supplement.

gr_weights.grd : greece_full_uncert.grd
	gmt grdmath $< 0 GT 0 DENAN = $@

greece_full_uncert.grd : ../Greece/Hellas-geology_18-30_34-43_30s.grd greece_uncert.grd
	gmt grdmath ../Greece/Hellas-geology_18-30_34-43_30s.grd 0 GT 0.4 MUL greece_uncert.grd EXCH DENAN 0 DENAN = $@

greece_uncert.grd : greece_uncert.xy
	gmt xyz2grd -G$@ $< -I30s -R18/30/34/43

##################################################################
# Add in the hybrid regional California uncertainty data using
# grdmath's IFELSE, and then make all water values go to zero.

global_uncert_ca.grd : cali_sd_zeros.grd vs30_slope_uncert.grd landmask_land.grd
	gmt grdmath cali_sd_zeros.grd cali_sd_zeros.grd vs30_slope_uncert.grd IFELSE landmask_land.grd MUL = $@

##################################################################
# This section takes the resampled map of Cali uncertainties and 
# pastes chunks of zeros to all four sides. Unforuntely this is the 
# easiest way to overlay it on the slope-based uncertainty map
# since we're not doing any blending at borders this time.
#
# As a side note, it seems like the insert_grd function
# adds a small border without the blending step?
#
# Second side note, after using insert_grd on the Greece map
# above that seems to not be the case (and the border is 
# still present after inserting California with this method
# below). So, probably will rewrite to use insert_grd for
# all of the maps at once at the end.

cali_sd_zeros.grd : cali_sd_n_s_e.grd west.grd
	gmt grdpaste west.grd cali_sd_n_s_e.grd -G$@
cali_sd_n_s_e.grd : cali_sd_n_s.grd east.grd
	gmt grdpaste cali_sd_n_s.grd east.grd -G$@
cali_sd_n_s.grd : cali_sd_n.grd south.grd
	gmt grdpaste cali_sd_n.grd south.grd -G$@
cali_sd_n.grd : cali_sd.grd north.grd
	gmt grdpaste cali_sd.grd north.grd -G$@

north.grd :
	gmt grdmath -R$(ZEROS_N) -I$(RES)s/$(RES)s 0 1 NAN = $@
south.grd :
	gmt grdmath -R$(ZEROS_S) -I$(RES)s/$(RES)s 0 1 NAN = $@
west.grd :
	gmt grdmath -R$(ZEROS_W) -I$(RES)s/$(RES)s 0 1 NAN = $@
east.grd :
	gmt grdmath -R$(ZEROS_E) -I$(RES)s/$(RES)s 0 1 NAN = $@

###########################################################################
# Create a landmask - make all water 0, all land 1. This way, uncertainty
# in water is just 0, and on land we can use the assigned values.

landmask_land.grd :
	gmt grdlandmask -V -N0/1/0/1/0 -R$(GLOBAL_REGION) -I$(RES)s -G$@ -Df

################################################
# Check California resolution and resample.

ifeq ($(IS_COARSER),true)
cali_sd.grd : California_Vs30_7p5c_sd.grd
	gmt grdfilter -I$(RES)s/$(RES)s -R$(CALI_REGION) -T -D0 -Fg0.016 -fg $< -G$@
else
cali_sd.grd : California_Vs30_7p5c_sd.grd
	gmt grdsample -nl+t0.1 -I$(RES)s/$(RES)s -R$(CALI_REGION) -fg $< -G$@
endif

##################################################################################
# Make the Vs30 slope-based Uncertainty map based on Seyhan et al., 2014.

vs30_slope_uncert.grd : ../Slope/global_grad.grd
	gmt grdmath $< 0.0022 GE 0.23 MUL 0.2 ADD = $@

####################################
# Compile insert grid program.

../src/insert_grd :
	$(MAKE) -C ../src insert_grd

######################################################################################
# Make the plots.

vs30_slope_uncert : vs30_slope_uncert.png

global_uncert_ca : global_uncert_ca.png

cali_sd : cali_sd.png

global_uncert_ca_gr : global_uncert_ca_gr.png

# Set some initial flags for plotting. Edit these to change the plotting specs for
# every map below, or just edit them individually lower down. 

Jflags1 = M9i		# Set the projection. Mercator, 9 inches wide.
#Jflags1 = N0/9i
Bflags1 = a20WSen	# Axis labels for the whole plot. One label every 2 degrees, only plot on W and S side.
Sflags = 128/128/255	# Make wet areas this color. Good blue color for water. 
Bflags1a = af		# Axis labels for the scale.
Cflags = ../Misc/uncert.cpt 
Dflags = 24/5/9/0.5	# Position(x,y), length, and width of the scale (all in cm).
Xflags = c		# X pos. of the map.
Yflags = c		# Y pos. of the map.
Tflags = g		# png
Eflags1 = 300		# dpi

vs30_slope_uncert.png : vs30_slope_uncert.grd
	gmt grdimage $< -J$(Jflags1) -R$(SMALLER_REGION) -C$(Cflags) -B$(Bflags1) -X$(Xflags) -Y$(Yflags) -K > vs30_slope_uncert.ps
	gmt pscoast -R$(SMALLER_REGION) -J$(Jflags1) -B -Df -S$(Sflags) -X$(Xflags) -Y$(Yflags) -O -K >> vs30_slope_uncert.ps
	gmt psscale -D$(Dflags) -B$(Bflags1a) -C$(Cflags) -O >> vs30_slope_uncert.ps
	gmt psconvert -E$(Eflags1) -P -T$(Tflags) vs30_slope_uncert.ps

Jflags2 = M6i
Bflags2 = a10f5
Eflags2 = 1080

global_uncert_ca.png : global_uncert_ca.grd
	gmt grdimage $< -J$(Jflags1) -R$(SMALLER_REGION) -C$(Cflags) -B$(Bflags2) -X$(Xflags) -Y$(Yflags) -K > global_uncert_ca.ps
	gmt pscoast -R$(SMALLER_REGION) -J$(Jflags1) -B -Df -S$(Sflags) -X$(Xflags) -Y$(Yflags) -O -K >> global_uncert_ca.ps
	gmt psscale -D$(Dflags) -B$(Bflags1a) -C$(Cflags) -O >> global_uncert_ca.ps
	gmt psconvert -E$(Eflags2) -P -T$(Tflags) global_uncert_ca.ps

cali_sd.png : cali_sd.grd
	gmt grdimage $< -J$(Jflags2) -R$(CALI_REGION) -C$(Cflags) -B$(Bflags2) -X$(Xflags) -Y$(Yflags) -K > cali_sd.ps
	gmt pscoast -R$(CALI_REGION) -J$(Jflags2) -B -S$(Sflags) -Df -Ia/$(Sflags) -X$(Xflags) -Y$(Yflags) -O -K >> cali_sd.ps
	gmt psscale -D$(Dflags) -B$(Bflags2) -C$(Cflags) -O >> cali_sd.ps
	gmt psconvert -E$(Eflags2) -P -T$(Tflags) cali_sd.ps

Bflags3 = a5f1

global_uncert_ca_gr.png : global_uncert_ca_gr.grd
	gmt grdimage $< -J$(Jflags1) -R$(GREECE_REGION) -C$(Cflags) -B$(Bflags3) -X$(Xflags) -Y$(Yflags) -K > global_uncert_ca_gr.ps
	gmt pscoast -R$(GREECE_REGION) -J$(Jflags1) -B -Df -S$(Sflags) -X$(Xflags) -Y$(Yflags) -O -K >> global_uncert_ca_gr.ps
	gmt psscale -D$(Dflags) -B$(Bflags1a) -C$(Cflags) -O >> global_uncert_ca_gr.ps
	gmt psconvert -E$(Eflags2) -P -T$(Tflags) global_uncert_ca_gr.ps
	rm gmt.history
