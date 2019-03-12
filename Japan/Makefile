#
# Makefile for including Japan Vs30 grid into the global grid
#

include ~/.vs30/Constants.mk

JP_XMIN = 128.0
JP_XMAX = 149.0
JP_YMIN = 31.0
JP_YMAX = 46.0
JP_REGION = $(JP_XMIN)/$(JP_XMAX)/$(JP_YMIN)/$(JP_YMAX)
SHAPE_BASE = Z-V2-JAPAN-AMP-VS400_M250-SHAPE

.PHONY: all plots clean clean_plots veryclean

all : japan.grd weights.grd

plots :  plot_raw_map plot_weights plot_final_map

clean : clean_plots
	$(RM) japan.grd weights.grd shape/japan_$(RES)c.bil \
		  shape/japan_$(RES)c.hdr shape/japan_$(RES)c.prj \
		  shape/$(SHAPE_BASE).dbf shape/$(SHAPE_BASE).prj \
		  shape/$(SHAPE_BASE).shp  shape/$(SHAPE_BASE).shx gmt.history

clean_plots :
	$(RM) *.ps *.png
	$(RM) japan_region.grd

veryclean : clean
	$(RM) -rf shape

#################################################################
# Make a weighted clipping mask; this is trivial in this case
# because our region is entirely surrounded by water; the 
# weights = 1 where we have Vs30, = 0 where we don't 
#

weights.grd : japan.grd 
	gmt grdmath $< 0 GT = $@

#################################################################
# Make the raster file into a GMT grd
#

japan.grd : shape/japan_$(RES)c.bil
	gmt xyz2grd -R$(JP_REGION) -I$(RES)s -ZTLf $< -G$@
#
# Convert shape file into a raster file
#

shape/japan_$(RES)c.bil : shape/$(SHAPE_BASE).shp
	$(GDAL_PATH)/gdal_rasterize -of EHdr -a AVS -init 0 -te $(JP_XMIN) $(JP_YMIN) $(JP_XMAX) $(JP_YMAX) -tr $(RES_DD) $(RES_DD) -ot Float32 -tap $< $@
#
# Get the grid from the source and unpack it 
#

shape/$(SHAPE_BASE).shp : shape/$(SHAPE_BASE).tar.gz
	(cd shape && tar xvf $(SHAPE_BASE).tar.gz && touch -c $(SHAPE_BASE).shp)

shape/$(SHAPE_BASE).tar.gz : 
	mkdir -p shape;
	$(CURL) http://www.j-shis.bosai.go.jp/map/JSHIS2/data/Z/V2/JAPAN/AMP/VS400_M250/$(SHAPE_BASE).tar.gz > shape/$(SHAPE_BASE).tar.gz

#################################
#
# Plots
#
plot_weights : weights.png

plot_final_map : japan_region.png

plot_raw_map : japan_raw.png

# Set some flags for plotting. To change things for an individual map,
# make sure to only change it in that particular code block below.

Jflags = M18		# Mercator projection, 18 cm across
Bflags = a4f2WSen	# Tick marks. Every 4 degrees deneoted with number, 2 degree tickmarks, print numbers on W and S axes
Dflags = 20/4.3/9/0.5	# Scalebar - position 20, 4.3. 9 cm long, 0.5 cm wide.
Eflags = 720		# 720 dpi resolution for psconvert
Tflags = g		# Output png. Use f for pdf, j for jpg

# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(NEW_WEIGHTS_CPT) -J$(Jflags) -B$(Bflags) -K > weights.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_WEIGHTS_CPT) -O -K >> weights.ps
	gmt pscoast -J$(Jflags) -R$(JP_REGION) -Df -O -N1 -N2 -W >> weights.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) weights.ps

# Plot the new Vs30 map:
 
japan_region.png : japan_region.grd
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > japan_region.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O -K >> japan_region.ps
	gmt pscoast -J$(Jflags) -R$(JP_REGION) -O -Df -N1 -N2 -W >> japan_region.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) japan_region.ps
	rm gmt.history

japan_region.grd : ../global_vs30.grd
	gmt grdcut -R$(JP_REGION) $< -G$@

# Plot the raw geology map using its own scale.

japan_raw.png : japan.grd
	gmt grdimage $< -J$(Jflags) -C$(NEW_VS30_CPT) -B$(Bflags) -K > japan_raw.ps
	gmt psscale -D$(Dflags) -L -C$(NEW_VS30_CPT) -O >> japan_raw.ps
	gmt psconvert -E$(Eflags) -P -T$(Tflags) japan_raw.ps
