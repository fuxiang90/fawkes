
/***************************************************************************
 *  main.cpp - main application for geegaw camera computer vision pipeline
 *
 *  Geegaw has been chosen for obvious reasons. The purpose of this
 *  application is to detect cups in the image and its position in
 *  coordinates relative to the robot.
 *
 *  Generated: Tue Apr 10 14:33:19 2007
 *  Copyright  2005-2007  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/// @cond RCSoftX

#include <iostream>

#include <utils/config_reader/config_reader.h>

#include "pipeline.h"
#include "bbclient.h"
#include "config.h"

#include <utils/system/argparser.h>
#include <utils/system/console_colors.h>

#include <fvutils/ipc/shm_image.h>
#include <fvutils/ipc/shm_registry.h>

using namespace std;

int
main( int argc, char **argv )
{

  // see help output below for the meaning of the parameters
  ArgumentParser *argp = new ArgumentParser(argc, argv, "f:Lb::x:Hoi:r:l:pvP");

  // wipe away all shared memory segments that we are responsible for.
  SharedMemoryImageBuffer::wipe( FIREVISION_SHM_IMAGE_OMNI_RAW );  
  SharedMemoryImageBuffer::wipe( FIREVISION_SHM_IMAGE_OMNI_PROCESSED );


  if (argp->hasArgument("H")) {

    cout << endl << cblue << "FirevisionFront Help" << cnormal << endl << endl
         << " -o        Output debug information" << endl
	 << " -p        Show pose (localize) information" << endl
         << " -P        Show pan/tilt information" << endl
	 << " -b[delay] Do NOT run as BBClient but standalone, optional delay dly ms" << endl
	 << " -x file   use named XML config file" << endl
	 << " -L        Use FileLoader instead of real cam" << endl
	 << " -f file   File to load with FileLoader" << endl
         << " -i robot  Connect to remote robot BB, mutually exclusive with -b" << endl
         << " -r robot  Connect to remote robot BB, mutually exclusive with -b" << endl
         << " -l.       Connect to local BB, mutually exclusive with -b" << endl
	 << " -H        Shows this help" << endl
	 << endl
	 << "Examples:" << endl
	 << endl
	 << " firevision_geegaw -b" << endl
	 << " Run front vision without blackboard functionality" << endl << endl
	 << " firevision_front -bd -l -f file.raw -F dual_sobel" << endl
	 << " Run front vision with debug display and use file loader instead of real" << endl
	 << " camera" << endl
	 << endl;

  } else if (argp->hasArgument("b")) {
    // Do NOT run bbclient

    cout << cblue << "FirevisionGeegaw" << cnormal << ": Running pipeline directly" << endl;

    CConfigReader *cr = new CConfigReader();

    char *config_filename;
    if ( (config_filename = argp->getArgument("x")) == NULL ) {
      config_filename = "../cfg/config.xml";
      cout << cblue << "FirevisionGeegaw" << cnormal
	   << ": No config file given, using default ("
	   << config_filename << ")" << endl;
    }
    cr->LoadConfFile( config_filename );
    GeegawConfig *c = new GeegawConfig( cr );

    char *tmp;
    int delay = 0;
    if ( (tmp = argp->getArgument("b")) != NULL ) {
      delay = atoi(tmp);
      if (delay < 0) {
	cout << cblue << "FirevisionGeegaw: " << cred << "Invalid delay. Resetting to 0" << cnormal << endl;
	delay = 0;
      }
    }

    cout << cblue << "FirevisionGeegaw" << cnormal << ": Delay is " << delay << " ms" << endl;

    GeegawPipeline *fp = new GeegawPipeline(argp, c, false);

    fp->init();
    fp->run( delay /* delay in ms */);
    fp->finalize();

    delete fp;
    delete cr;
    delete c;

  } else {
    // Run bbclient

    cout << cblue << "FirevisionGeegaw" << cnormal << ": Running BBClient" << endl;

    FirevisionGeegawBBClient *bbclient = new FirevisionGeegawBBClient(argc, argv, argp);
    bbclient->Main();
    delete bbclient;

  }

  delete argp;

}

/// @endcond