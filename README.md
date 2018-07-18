earthquake-global_vs30
===================

Global Vs30 model based on topographic slope, with custom embedded
maps.

Author: C. Bruce Worden 

Copyright (c) 2014 United States Geological Survey

See LICENSE file for terms and conditions.


GLOBAL VS30 MAP
================

The software in this repository creates a global topographic-slope
based Vs30 model, and then inserts regional maps into the global
map where they are available.

It starts running in the Slope directory, where it makes a file
called "global_vs30.grd" and copies it to the top-level directory.
It then runs in each of the regional sub-directories (currently,
California, PNW, Utah, Japan, Taiwan, New Zealand, and Australia) each of which
 inserts its regional map into the global map, tagging it with a
 "_<region>" extension (e.g., California uses the "ca" extension, so
 if it were to operate on the "global_vs30.grd" file, it would create
 a file called "global_vs30_ca.grd". The top-level file is then removed,
and the region's custom file is copied into its place. The
next region then operates on that file (e.g., PNW, whose extension
is "waor" would act on "global_vs30_ca.grd" and create
"global_vs30_ca_waor.grd".)

In this way, each stage of the process is captured in "Slope"
and the regional subdirectories, while the final product lives
in the top-level directory. This approach should make it easy to
add new regions, and to skip certain regions, without too much
custom programming.


PREREQUISITES AND CONFIGURATION
------------------------------------------------

You will need to have certain software installed and within your
current search path. The required software is:

+ **CURL** -- (or curl) a utility to transfer data from a server via HTTP or
other protocols; if it isn't on your system, you can install it with a
package manager, or it can be had from various sources, but the main
one is http://curl.haxx.se.


+ **GMT/NetCDF** -- The Generic Mapping Tools package must be installed, and
NetCDF with it. GMT can be downloaded from: http://gmt.soest.hawaii.edu/.
Get version 4 -- this software hasn't been tested with version 5. Make
sure to install the full-resolution shoreline database.


+ The **GDAL (Geospatial Data Abstraction Library)** package must be available.
You will need at least version 1.9 -- earlier versions, like 1.4, will not
work. Additionally, the most recent version -- 2.3.0, is incompatible.
You will need to set a path to the gdal binaries in Constants.mk.
In particular, the programs gdal_rasterize and gdal_translate are used
herein. GDAL can be installed by most package managers, but they often
have very old versions. The software can also be found at www.gdal.org.


+ If you have runtime problems with the C programs ("smooth", "insert_grd",
"grad2vs30"), have a look at the **STATIC** variable in Constants.mk


USER-DEFINED CONSTANTS.MK
_________________________

Before starting, you should notice the hidden directory ".vs30" included in the 
repository. Please move this directory and its contents to your home directory (~). 
Review the user-specific constants in ~/.vs30/Constants.mk. In
particular, make sure CDFLIB, CDFINC, GMTLIB, and GMTINC are set
correctly for your system. You'll also want to set the resolution
of the maps to one of the supported values; 30 arc-seconds is the
default. Note that the 7.5 second resolution makes very large files
and the GMT commands require system memory in excess of what most
machines can currently support. See Slope/commands_75.txt for a list
of commands that will make the 7.5 second map in two pieces, and
then join them at the end.


MAKING THE MAPS
------------------------

Once all of the proper software is installed, and the variables in
Constants.mk are set, in the top-level directory you should just be able to do:

        % make

And the process should proceed to make a global grid file with insert
maps for California, PNW, Utah, etc. To remove one or more regions, see
the top-level Makefile and look at the instructions near the top.

If you want to make plots, it's set up in the regional Makefiles. Doing:

        % make plots

will generate a complete set. To get rid of the plots, do:

        % make clean_plots

To get rid of a lot of intermediate products (and all the plots), do:

        % make clean

To get rid of everything except the stuff that can't be regenerated or
automatically downloaded, do:

        % make veryclean

You can do "make clean" and "make veryclean" from the top-level directory,
in which case it runs those commands recursively in all the subdirectories.
In the top-level you can also run:

        % make spotless

which will run "make veryclean" in all the subdirectories and also remove
the top level Vs30 file "global_vs30*.grd".


ADDING A REGION
-----------------------
To add a region, make a directory for it and add the directory to the
lists near the top of the top-level Makefile. The copy the Makefile
from one of the regions (California, PNW, or Utah) and modify as
necessary. In particular, you can make your map in any way you want, so
many of the steps in creating the the grids may be different or unnecessary.
You may also insert your grid into the global grid however you want,
but the program src/insert_grd is provided for that purpose. It takes as
input a base grid file into which you want to insert your map, your grid,
a landmask, and a weighted clipping mask.  The landmask and clipping mask
should be identically sized and co-registered with your grid. All three must
have the same resolution and co-register with the background grid, and must
fit entirely within it. The clipping mask should be 1 (one) where your grid
will replace the background grid, and 0 (zero) where the background grid is
left untouched. Values between 1 and 0 will produce and output that is the
weighted average of the two grids:

        output = w * insert_grd + (1 - w) * background_grd

where w is the weight.

See the file src/README and the source .c files for more information on
insert_grd and the other programs available. In addition, the regional
Makefiles (for California, PNW, and Utah) have examples of their use.


ACKNOWLEGEMENTS
---------------
The California map was provided by Eric Thompson and based on the map
developed in Thompson et al. (2014) "A Vs30 Map for California with Geologic
and Topographic Constraints." BSSA, in press.

The Utah map was provided by Kris Pankow of UUSS.

The Washington/Oregon map was provided by Renate Hartog of UW.

The map of Japan is downloaded from http://www.j-shis.bosai.go.jp/ (see
the Japan Makefile for specifics of downloading). References are available
at the web site.

The Taiwan maps were provided by Eric Thompson based upon maps developed
in Thompson & Wald (2012) "Developing Vs30 Site-Condition Maps by Combining
Observations with Geologic and Topographic Constraints." Presented at
15WCEE, Lisbon, Portugal.

The Australia map is provided by A. A. McPherson and Trevor Allen of Geoscience 
Australia under the Creative Commons Attribution 4.0 International Licence.

The getpar library was created by Robert W. Clayton of the California
Institute of Technology, and later amended by several others (see
source files for history). It is included by permission.

