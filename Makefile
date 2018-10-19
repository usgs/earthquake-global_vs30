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
                  Taiwan \
                          NZ \
                          Australia \
                                 Italy

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
                        Italy    

MKDIRS_CLEAN = $(patsubst %,%.clean,$(MKDIRS))
MKDIRS_VCLEAN = $(patsubst %,%.vclean,$(MKDIRS))

.PHONY: all slope clean veryclean $(INSERT_MAPS) $(MKDIRS_CLEAN) $(MKDIRS_VCLEAN)

all : $(INSERT_MAPS) global_vs30.grd

plots : global_vs30_plot

global_vs30.grd : src/insert_grd Slope/global_vs30.grd \
	California/california.grd California/weights.grd \
	Australia/aus.grd Australia/weights.grd \
	Japan/japan.grd Japan/weights.grd\
	NZ/newzealand.grd NZ/weights.grd \
	PNW/pnw.grd PNW/weights.grd \
	Taiwan/taiwan.grd Taiwan/weights.grd \
	Utah/ut_ext.grd Utah/weights.grd \
	Italy/new_italy.grd Italy/weights.grd
	./src/insert_grd gin=Slope/global_vs30.grd gout=$@ \
		grid1=California/california.grd gmask1=California/weights.grd \
		grid2=Australia/aus.grd gmask2=Australia/weights.grd \
		grid3=Japan/japan.grd gmask3=Japan/weights.grd \
		grid4=NZ/newzealand.grd gmask4=NZ/weights.grd \
		grid5=PNW/pnw.grd gmask5=PNW/weights.grd \
		grid6=Taiwan/taiwan.grd gmask6=Taiwan/weights.grd \
		grid7=Utah/ut_ext.grd gmask7=Utah/weights.grd \
		grid8=Italy/new_italy.grd gmask8=Italy/weights.grd

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
#
# Make plot
#
global_vs30_plot : global_vs30.png

global_vs30.png : global_vs30.grd
	gmt grdimage $< -JM20 -R-180/180/-56/72 -CMisc/global.cpt -Ba24d/a12eWSen -K > global_vs30.ps
	gmt pscoast -JM20 -R-180/180/-56/72 -Di -N1 -W -S128/128/255 -A1000/0/2 -O -K >> global_vs30.ps
	gmt psscale -D21/4.3/9/0.5 -L -CMisc/global.cpt -O >> global_vs30.ps
	convert -rotate 90 -density 300x300 -crop 3000x1400+100+950 global_vs30.ps $@

