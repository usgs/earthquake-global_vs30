#
# Makefile to generate a global slope-based Vs30 file
#

include ~/.vs30/Constants.mk

GRNLND_WEST = -73.0833333333
GRNLND_EAST = -12.125
GRNLND_NORTH = 83.675
GRNLND_SOUTH = 59.725

GRNLND_REGION = $(GRNLND_WEST)/$(GRNLND_EAST)/$(GRNLND_SOUTH)/$(GRNLND_NORTH)

GLOBAL_WEST = -180
GLOBAL_EAST = 180
GLOBAL_NORTH = 84
GLOBAL_SOUTH = -56

.PHONY: all clean veryclean

all : new_global_vs30.grd

plots : plot_cratons plot_cratons_smooth plot_global_topo plot_global_vs30

clean : clean_plots
	$(RM) cratons.bil cratons.prj cratons.hdr cratons.grd cratons_smooth.grd \
              global_landmask.grd elev/gmted_global.bil gmted_global.grd \
              global_grad.grd global_vs30_no_greenland.grd cratons_pixel.grd gmt.history

clean_plots:
	$(RM) *.ps *.png

veryclean : clean clean_plots
	$(RM) -rf elev
	$(RM) -rf cratons
	$(RM) global_vs30.grd

#######################
#
# Edit the map to make Greenland 601 m/s. This used to be a post-processing step.
#


new_global_vs30.grd : global_vs30_no_greenland.grd grnlnd_nan.grd
	gmt grdmath grnlnd_nan.grd global_vs30_no_greenland.grd AND = $@

grnlnd_nan.grd : grnlnd_nan_wns.grd east_nan.grd
	gmt grdpaste grnlnd_nan_wns.grd east_nan.grd -G$@
grnlnd_nan_wns.grd : grnlnd_nan_ns.grd west_nan.grd
	gmt grdpaste grnlnd_nan_ns.grd west_nan.grd -G$@
grnlnd_nan_ns.grd : grnlnd_nan_s.grd north_nan.grd
	gmt grdpaste grnlnd_nan_s.grd north_nan.grd -G$@
grnlnd_nan_s.grd : grnlnd_601_edit.grd south_nan.grd
	gmt grdpaste grnlnd_601_edit.grd south_nan.grd -G$@

west_nan.grd :
	gmt grdmath -R$(GLOBAL_WEST)/$(GRNLND_WEST)/$(GLOBAL_SOUTH)/$(GLOBAL_NORTH) -I$(RES)s/$(RES)s 0 0 NAN = $@
east_nan.grd :
	gmt grdmath -R$(GRNLND_EAST)/$(GLOBAL_EAST)/$(GLOBAL_SOUTH)/$(GLOBAL_NORTH) -I$(RES)s/$(RES)s 0 0 NAN = $@
north_nan.grd :
	gmt grdmath -R$(GRNLND_WEST)/$(GRNLND_EAST)/$(GRNLND_NORTH)/$(GLOBAL_NORTH) -I$(RES)s/$(RES)s 0 0 NAN = $@
south_nan.grd :
	gmt grdmath -R$(GRNLND_WEST)/$(GRNLND_EAST)/$(GLOBAL_SOUTH)/$(GRNLND_SOUTH) -I$(RES)s/$(RES)s 0 0 NAN = $@


grnlnd_601_edit.grd : grnlnd_601.grd
	gmt grdedit $< -A -R$(GRNLND_REGION) -G$@

grnlnd_601.grd : greenland_elev.grd
	gmt grdmath $< -R$(GRNLND_REGION) -11 GT 601 MUL = $@

# 
# Create the global slope-based Vs30 map.
#
global_vs30_no_greenland.grd : global_grad.grd global_landmask.grd cratons_smooth.grd \
                  ../src/grad2vs30
	../src/grad2vs30 gradient_file=global_grad.grd craton_file=cratons_smooth.grd landmask_file=global_landmask.grd output_file=global_vs30_no_greenland.grd water=$(WATER)

#
# Create the slope file from the DEM (the -G option isn't necessary on 
# newer versions of GMT):
#
global_grad.grd : gmted_global.grd
	gmt grdgradient $< -n+bg -fg -D -S$@ -Gjunk.grd -V
	$(RM) junk.grd

#
# Convert .bil to pixel registered .grd, then convert it to gridline 
# node registration:
#
gmted_global.grd : elev/gmted_global.bil
	gmt xyz2grd -ZTLh -r -R$(GLOBAL_REGION) -I$(RES)s $< \
                -Ggmted_global_pixel.grd
	gmt grdsample gmted_global_pixel.grd -G$@ -T -fg
	$(RM) gmted_global_pixel.grd

#
# Convert the DEM to .bil:
#
elev/gmted_global.bil : elev/md$(GRES)_grd
	$(GDAL_PATH)/gdal_translate -of EHdr $< $@

#
# Unzip the DEM; we have to touch the dir and files because they
# are older than the archive that contains them, forcing make
# to always repeat this (and all following) step(s)
#
elev/md$(GRES)_grd : elev/md$(GRES)_grd.zip
	(cd elev && unzip md$(GRES)_grd.zip && touch -c md$(GRES)_grd && \
         touch md$(GRES)_grd/* )

#
# Get the DEM:
#
elev/md$(GRES)_grd.zip :
	mkdir -p elev;
	$(CURL) https://edcintl.cr.usgs.gov/downloads/sciweb1/shared/topo/downloads/GMTED/Grid_ZipFiles/md$(GRES)_grd.zip > $@

#
# Create the landmask:
#
global_landmask.grd : 
	gmt grdlandmask -V -R$(GLOBAL_REGION) -I$(RES)s -G$@ -Df

#
# Smooth the craton file
#
cratons_smooth.grd : cratons.grd ../src/smooth
	../src/smooth infile=cratons.grd fx=$(GLOBE_FX) fy=$(GLOBE_FY) outfile=$@


#
# Convert the craton.bil to a pixel registered .grd file, then convert that to gridline 
# node registration; use -nn to keep everything at 0 and 1:
#
cratons.grd : cratons.bil
	gmt xyz2grd -R$(GLOBAL_REGION) -I$(RES)s -ZTLc -r $< -Gcratons_pixel.grd
	gmt grdsample cratons_pixel.grd -G$@ -T -fg -nn
	$(RM) cratons_pixel.grd

#
# Create the craton file from the shape files:
#
cratons.bil : cratons/cratons_nshmp.shp
	$(GDAL_PATH)/gdal_rasterize -burn 1 -of EHdr -init 0 -te $(GXMIN) $(GYMIN) $(GXMAX) $(GYMAX) -tr $(RES_DD) $(RES_DD) -ot Byte $< $@

cratons/cratons_nshmp.shp : cratons.tgz
	tar xvmf $<

cratons.tgz :
	echo "File cratons.tgz must be supplied -- cannot make"


#
# Make the C programs
#
../src/smooth :
	$(MAKE) -C ../src smooth

../src/grad2vs30 :
	$(MAKE) -C ../src grad2vs30

######################################################################
# Make some plots
######################################################################
plot_cratons : cratons.png
	
plot_cratons_smooth : cratons_smooth.png

plot_global_topo : global_topo.png

plot_global_vs30 : global_vs30.png

# Set some flags for plotting. To change things for individual plots,
# make sure to only change it in the code block below.

Jflags = M18		# Mercator projection, 18 cm across
Bflags = a24f12eWSn	# Tick marks. Every 2 degrees deneoted with number, 1 degree tickmarks, print numbers on W and S axes
Dflags = 19/4.3/9/0.5	# Scalebar - position 19, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg
Sflags = 128/128/255	# Fill wet areas with this color.
Aflags = 1000/0/2	# Don't plot things smaller than 1000 km^2
Rflags1 = -180/180/-56/72	# Larger region to plot
Rflags2 = -130/-48/12/50	# Smaller region to plot
Cflags1 = ../Misc/tan.cpt	# Location of tan cpt
Cflags2 = ../Misc/global.cpt	# Location of global cpt

cratons.png : cratons.grd
	gmt grdimage cratons.grd -J$(Jflags) -R$(Rflags1) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > cratons.ps
	gmt pscoast -J$(Jflags) -R$(Rflags1) -Df -N1 -W -S$(Sflags) -A$(Aflags) -O -K >> cratons.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O >> cratons.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) cratons.ps

cratons_smooth.png : cratons_smooth.grd
	gmt grdimage cratons_smooth.grd -J$(Jflags) -R$(Rflags2) -C$(NEW_WEIGHTS_CPT) -B$(Bflags) -K > cratons_smooth.ps
	gmt pscoast -J$(Jflags) -R$(Rflags2) -Df -N1 -N2 -W -S$(Sflags) -A$(Aflags) -O -K >> cratons_smooth.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O >> cratons_smooth.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) cratons_smooth.ps

global_topo.png : gmted_global.grd
	gmt grdimage gmted_global.grd -J$(Jflags) -R$(Rflags1) -C$(Cflags1) -B$(Bflags) -K > global_topo.ps
	gmt pscoast -J$(Jflags) -R$(Rflags1) -Df -N1 -W -S$(Sflags) -A$(Aflags) -O -K >> global_topo.ps
	gmt psscale -D$(Dflags) -L -C$(Cflags1) -O >> global_topo.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) global_topo.ps

global_vs30.png : global_vs30.grd
	gmt grdimage global_vs30.grd -J$(Jflags) -R$(Rflags1) -C$(Cflags2) -B$(Bflags) -K > global_vs30.ps
	gmt pscoast -J$(Jflags) -R$(Rflags1) -Df -N1 -W -S$(Sflags) -A$(Aflags) -O -K >> global_vs30.ps
	gmt psscale -D$(Dflags) -L -C$(Cflags2) -O >> global_vs30.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) global_vs30.ps
	rm gmt.history
