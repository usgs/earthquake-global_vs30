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

# Plot the weights:
 
weights.png : weights.grd
	gmt grdimage weights.grd -C$(WEIGHTS_CPT) -JM12 -Ba4f2eSWn -K > weights.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(WEIGHTS_CPT) -O -K >> weights.ps
	gmt pscoast -JM12 -R$(JP_REGION) -Df -O -N1 -N2 -W >> weights.ps
	convert -trim -rotate 90 -density 300x300 weights.ps weights.png
 
# Plot the new Vs30 map:
 
japan_region.png : japan_region.grd
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba4f2eSWn -K > japan_region.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O -K >> japan_region.ps
	gmt pscoast -JM12 -R$(JP_REGION) -O -Df -N1 -N2 -W >> japan_region.ps
	convert -trim -density 300x300 -rotate 90 japan_region.ps japan_region.png

japan_region.grd : ../global_vs30.grd
	gmt grdcut -R$(JP_REGION) $< -G$@

# Plot the raw geology map using its own scale.

japan_raw.png : japan.grd
	gmt grdimage $< -JM12 -C$(VS30_CPT) -Ba4f2eSWn -K > japan_raw.ps
	gmt psscale -D14/4.3/9/0.5 -L -C$(VS30_CPT) -O >> japan_raw.ps
	convert -trim -rotate 90 -density 300x300 japan_raw.ps japan_raw.png

