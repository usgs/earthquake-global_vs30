###############################################################################################
# Top-level Makefile for the Vs30 project
#
# Previous versions of this software allowed you to add regions to the global map 
# one at a time. However, this version now limits that ability (due to computational
# restrictions) so that you can only add the regions listed below all at once. 
#
# Often, at least one plot in each of the regional directories' Makefiles requires 
# global_vs30.grd to exist before the plot will be created. You can comment that plot out 
# when first adding a new region, or create that file in the Slope directory by typing `make`
# and copying it to the top level directory.
#

################################################################################################
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
                  Taiwan \
                          NZ \
                          Australia \
                                 Italy \
                                 Iran \
                                 Greece
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
         Taiwan \
                 NZ \
                 Australia \
                        Italy \
                        Iran \
                        Greece

MKDIRS_CLEAN = $(patsubst %,%.clean,$(MKDIRS))
MKDIRS_VCLEAN = $(patsubst %,%.vclean,$(MKDIRS))

.PHONY: all slope clean veryclean $(INSERT_MAPS) $(MKDIRS_CLEAN) $(MKDIRS_VCLEAN)

all : $(INSERT_MAPS) global_vs30.grd

plots : global_vs30_plot

# Make sure to edit this section to reflect the regions listed above. Just follow 
# the format / naming conventions and it should work without a problem. Keep in mind
# both the weighted clipping mask and the new Vs30 grid need to be the same size and
# co-registered. 

global_vs30.grd : src/insert_grd Slope/global_vs30.grd \
	California/california.grd California/weights.grd \
	Australia/aus.grd Australia/weights.grd \
	Japan/japan.grd Japan/weights.grd\
	NZ/newzealand.grd NZ/weights.grd \
	PNW/pnw.grd PNW/weights.grd \
	Taiwan/taiwan.grd Taiwan/weights.grd \
	Utah/ut_ext.grd Utah/weights.grd \
	Italy/new_italy.grd Italy/weights.grd \
	Iran/iran.grd Iran/weights.grd \
	Greece/greece.grd Greece/weights.grd
	./src/insert_grd gin=Slope/global_vs30.grd gout=$@ \
		grid1=California/california.grd gmask1=California/weights.grd \
		grid2=Australia/aus.grd gmask2=Australia/weights.grd \
		grid3=Japan/japan.grd gmask3=Japan/weights.grd \
		grid4=NZ/newzealand.grd gmask4=NZ/weights.grd \
		grid5=PNW/pnw.grd gmask5=PNW/weights.grd \
		grid6=Taiwan/taiwan.grd gmask6=Taiwan/weights.grd \
		grid7=Utah/ut_ext.grd gmask7=Utah/weights.grd \
		grid8=Italy/new_italy.grd gmask8=Italy/weights.grd \
		grid9=Iran/iran.grd gmask9=Iran/weights.grd \
		grid10=Greece/greece.grd gmask10=Greece/weights.grd

clean : $(MKDIRS_CLEAN)

veryclean : $(MKDIRS_VCLEAN)

spotless : veryclean
	$(RM) global_vs30.grd

$(INSERT_MAPS) :
	$(MAKE) -C $@

$(MKDIRS_CLEAN) :
	$(MAKE) -C $(@:.clean=) clean

$(MKDIRS_VCLEAN) :
	$(MAKE) -C $(@:.vclean=) veryclean

src/insert_grd :
	$(MAKE) -C src insert_grd

######################
# Make plot
#

global_vs30_plot : global_vs30.png

global_vs30.png : global_vs30.grd
	gmt grdimage $< -JM20 -R-180/180/-56/72 -CMisc/global.cpt -Ba24d/a12eWSen -K > global_vs30.ps
	gmt pscoast -JM20 -R-180/180/-56/72 -Di -N1 -W -S128/128/255 -A1000/0/2 -O -K >> global_vs30.ps
	gmt psscale -D21/4.3/9/0.5 -L -CMisc/global.cpt -O >> global_vs30.ps
	convert -rotate 90 -density 300x300 -crop 3000x1400+100+950 global_vs30.ps $@

