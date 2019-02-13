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
that contains a global topographic slope-based Vs30 map.
It then runs in each of the regional sub-directories (currently 
Australia, California, Greece, Iran, Italy, Japan, New Zealand, PNW, 
Taiwan, and Utah) each of which inserts its regional map 
into the global map.

Each stage of the process is captured in "Slope"
and the regional subdirectories, while the final product, "global_vs30.grd" 
lives in the top-level directory. This approach should make it easy to
add new regions.


PREREQUISITES AND CONFIGURATION
------------------------------------------------

You will need to have certain software installed and within your
current search path. The required software is:

+ **CURL** -- (or curl) a utility to transfer data from a server via HTTP or
other protocols; if it isn't on your system, you can install it with a
package manager, or it can be obtained from various sources, but the main
one is http://curl.haxx.se.


+ **GMT/NetCDF** -- The Generic Mapping Tools package must be installed, and
NetCDF with it. GMT can be downloaded from: http://gmt.soest.hawaii.edu/.
The latest stable version -- 5.4.4 -- should be downloaded. **Note** GMT version 
4 will no longer work. Make sure to install the full-resolution shoreline database.


+ The **GDAL (Geospatial Data Abstraction Library)** package must be available.
You will need at least version 1.9 -- earlier versions, like 1.4, will not
work. Additionally, the most recent version -- 2.3.0, is incompatible.
You will need to set a path to the gdal binaries in Constants.mk, so remember where
you put it. In particular, the programs gdal_rasterize and gdal_translate are used
herein. GDAL can be installed by most package managers, but they often
have very old versions. The software can also be found at www.gdal.org.


+ **ImageMagick** must be installed in order to view the plots created to spot-check
the grid creation (using the command "convert"). This code is tested with version 7.0.8-7. 
Please make sure to follow the installation instructions to ensure your path points 
to the correct location. 


+ If you have runtime problems with the C programs ("smooth", "insert_grd",
"grad2vs30"), have a look at the **STATIC** variable in Constants.mk


USER-DEFINED CONSTANTS.MK
----------------------------

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
Constants.mk are set, in the top-level directory of the repository
you should just be able to do:

        % make

And the process should proceed to make a global grid file with insert
maps for Australia, California, Greece, Iran, Italy, Japan, NZ, PNW, 
Taiwan, Utah, etc. To remove (or add) one or more regions, see
the top-level Makefile and look at the instructions near the top.

If you want to make plots, it's set up in the regional Makefiles. 
Go into a regional directory and do:

        % make plots

to generate a complete set. These plots are sometimes limited (and vary from 
region to region), or may not show the step you are curious about. It is 
straightforward to add a new plot by copying the framework already there.
To get rid of the plots, do:

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
the top level Vs30 file "global_vs30.grd".


ADDING A REGION
-----------------------
To add a region, make a directory for it and add the directory to the two
lists near the top of the top-level Makefile. Then copy the Makefile
from one of the other regions (ideally one geographically similar. i.e., if it is a 
landlocked region, Utah might be best. Island - Australia or Taiwan. Regions with
coastline - Greece or Italy) and modify as necessary. In particular, you 
can make your map in any way you want, so many of the steps in creating the the 
grids may be different or unnecessary. There are many "branching points" in making the maps, 
and many small adjustments that can be made to correct for inconsistencies in the ways 
different maps are created. You may also insert your grid into the global 
grid however you want, but the program src/insert_grd is provided for that purpose. 
It takes as input a base grid file into which you want to insert your map (in our case the 
topographic slope grid), your new grid, and a weighted clipping mask (usually weights.grd). 
The clipping mask should be identically sized and co-registered with your grid. Both the new
grid and the weighted clipping mask must have the same resolution and co-register with the 
background grid, and must fit entirely within it. The clipping mask should be 1 (one) where 
your grid will completely replace the background grid, and 0 (zero) where the background grid is
left untouched. Values between 1 and 0 will produce and output that is the
weighted average of the two grids:

        output = w * insert_grd + (1 - w) * background_grd

where w is the weight.

See the file src/README and the source .c files for more information on
insert_grd and the other programs available. In addition, the regional
Makefiles (for California, PNW, and Utah) have examples of their use.


AMPLIFICATION MAP
=======================
This is a new addition to the Vs30 repository. The Amplification map is created separately
from all other regional / global Vs30 maps. It only requires "global_vs30.grd" exist in the top-level
directory. The amplification map is created by first utilizing two methodologies for calculating 
amplification values - Stewart et al. (2017) for stable cratonic regions, and 
Seyhan and Stewart (2014) for active crustal regions. The two maps are calculated and then merged 
together according to a simple polylgon grid file of their respective regions (modified from the 
ShakeMap repository). However, to avoid a sharp discontinuity at the interface, the two regions are blended
using a similar methodology to inserting regional maps into the global topographic-slope Vs30 map. 
In this case, however, the "background" map is the flipped version of the active/stable amplification map. 
Then, at the interface, the two are averaged (using the weight scheme described just above this section)
so that it blends the two together at the interface. Presently, the values for a requested period must 
be hard-coded from supplementary tables included in the publications listed in this section.


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

The Italy map is provided by Alberto Michelini of the Istituto Nazionale di Geofisica e 
Vulcanologia (INGV), and is developed in Michelini et al. (2008). 

The Iran map is provided by Sadra Karimzadeh of the Tokyo Institute of Technology, and
is developed in Karimzadeh et al. (2017). 

The Greece map is provided by Kiriaki Konstantinidou of the Earthquake Planning and 
Protection Organization (EPPO), and can be referenced at Stewart et al. (2014).

The New Zealand map ...

The getpar library was created by Robert W. Clayton of the California
Institute of Technology, and later amended by several others (see
source files for history). It is included by permission.

