
/***************************************************************************
 *  bb2rectlut.cpp - BB2 Rectification LUT utility
 *
 *  Created: Mon Oct 29 19:04:28 2007
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

#ifdef HAVE_BUMBLEBEE2_CAM
#include <cams/bumblebee2.h>
#endif
#include <fvutils/system/camargp.h>
#include <utils/system/argparser.h>
#include <fvutils/rectification/rectfile.h>
#include <fvutils/rectification/rectinfo_block.h>
#include <fvutils/rectification/rectinfo_lut_block.h>

#ifdef HAVE_TRICLOPS_SDK
#include <stereo/triclops.h>
#endif


#include <cstdio>

void
print_usage(ArgumentParser *argp)
{
  printf("Usage: %s <-r|-v|-i> file.rectlut\n", argp->program_name());
  printf("You have to give at least one of -r/-v/-i and a file name\n"
	 "  -r   retrieve rectification lut from live camera,\n"
	 "       uses first found Bumblebee2 camera\n"
	 "  -v   verify rectification lut, compares the identification\n"
	 "       info stored in the file with the first currently\n"
	 "       attached camera\n"
	 "  -d   deep verifiction of rectification LUT, compares the identification\n"
	 "       info stored in the file with the first currently attached camera. It\n"
	 "       also verify each single mapping on equality.\n"
	 "  -i   print info about rectification LUT file\n\n"
	 );
  exit(1);
}


int
retrieve(ArgumentParser *argp)
{
#ifdef HAVE_BUMBLEBEE2_CAM
#ifdef HAVE_TRICLOPS_SDK
  const char *lut_file = argp->items()[0];

  if ( access(lut_file, F_OK) == 0) {
    fprintf(stderr, "File with name %s exists, delete manually and retry. Aborting.\n", lut_file);
    return -1;
  }
  if ( access(lut_file, W_OK) != 0) {
    // ENOENT is ok, we would have access, but there is no file, yet
    if ( errno != ENOENT ) {
      fprintf(stderr, "Cannot write to file %s, permission problem?\n", lut_file);
      return -2;
    }
  }

  CameraArgumentParser *cap = new CameraArgumentParser("bumblebee2:Bumblebee2 BB2-03S2C");
  Bumblebee2Camera *bb2 = new Bumblebee2Camera(cap);
  bb2->open();

  TriclopsStereoProcessor *triclops = new TriclopsStereoProcessor(bb2);
  triclops->generate_rectification_lut(lut_file);
  delete triclops;

  bb2->close();

  delete bb2;
  delete cap;
#else
  printf("Retrieving the rectification LUT from a camera is not supported,\n"
	 "because the Triclops SDK was not available at compile time.\n");
#endif
#else
  printf("Retrieving the rectification LUT from a camera is not supported,\n"
	 "because the Bumblebee2 support was not available at compile time.\n");
#endif

  return 0;
}


int
verify(ArgumentParser *argp)
{
  int rv = 0;

#ifdef HAVE_BUMBLEBEE2_CAM
  CameraArgumentParser *cap = new CameraArgumentParser("bumblebee2:Bumblebee2 BB2-03S2C");
  Bumblebee2Camera *bb2 = new Bumblebee2Camera(cap);
  bb2->open();

  for (unsigned int i = 0; i < argp->num_items(); ++i) {

    const char *lut_file = argp->items()[i];

    if ( access(lut_file, F_OK) != 0) {
      fprintf(stderr, "File with name %s does not exist. Ignoring.\n", lut_file);
      continue;
    }
    if ( access(lut_file, R_OK) != 0) {
      fprintf(stderr, "Cannot read file %s, permission problem? Ingoring.\n", lut_file);
      continue;
    }

    RectificationInfoFile *rif = new RectificationInfoFile();
    try {
      rif->read(lut_file);

      if ( bb2->verify_guid( rif->guid() ) ) {
	printf("Success. The rectification info file has been created for the "
	       "connected camera\n");
      } else {
	printf("Failure. The rectification info file has *not* been created "
	       "for the connected camera\n");
	rv = 5;
      } 
    } catch (Exception &e) {
      fprintf(stderr, "Failed to read lut file %s\n", lut_file);
      e.print_trace();
    }

    delete rif;

  }

  bb2->close();
    
  delete bb2;
  delete cap;
    
#else
  printf("Verifying the rectification LUT from a camera is not supported,\n"
	 "because the Bumblebee2 support was not available at compile time.\n");
#endif

  return rv;
}


int
deep_verify(ArgumentParser *argp)
{
#ifdef HAVE_BUMBLEBEE2_CAM
#ifdef HAVE_TRICLOPS_SDK
  int rv = 0;

  CameraArgumentParser *cap = new CameraArgumentParser("bumblebee2:Bumblebee2 BB2-03S2C");
  Bumblebee2Camera *bb2 = new Bumblebee2Camera(cap);
  bb2->open();

  TriclopsStereoProcessor *triclops = new TriclopsStereoProcessor(bb2);

  for (unsigned int i = 0; i < argp->num_items(); ++i) {

    const char *lut_file = argp->items()[i];

    if ( access(lut_file, F_OK) != 0) {
      fprintf(stderr, "File with name %s does not exist. Ignoring.\n", lut_file);
      continue;
    }
    if ( access(lut_file, R_OK) != 0) {
      fprintf(stderr, "Cannot read file %s, permission problem? Ingoring.\n", lut_file);
      continue;
    }

    if ( triclops->verify_rectification_lut(lut_file) ) {
      printf("Success. LUT file %s contains matching configuration data.\n", lut_file);
    } else {
      printf("Failure. LUT file %s does not contain matching configuration data.\n", lut_file);
    }

  }

  delete triclops;
  bb2->close();
    
  delete bb2;
  delete cap;
    
  return rv;
#else
  printf("Deep verification of the rectification LUT from a camera is not supported,\n"
	 "because the Triclops SDK was not available at compile time.\n");
  return 0;
#endif
#else
  printf("Deep verification of the rectification LUT from a camera is not supported,\n"
	 "because the Bumblebee2 support was not available at compile time.\n");
  return 0;
#endif
}


void
print_info(ArgumentParser *argp)
{
  for (unsigned int i = 0; i < argp->num_items(); ++i) {

    const char *lut_file = argp->items()[i];

    if ( access(lut_file, F_OK) != 0) {
      fprintf(stderr, "File with name %s does not exist. Ignoring.\n", lut_file);
      continue;
    }
    if ( access(lut_file, R_OK) != 0) {
      fprintf(stderr, "Cannot read file %s, permission problem? Ingoring.\n", lut_file);
      continue;
    }

    RectificationInfoFile *rif = new RectificationInfoFile();
    try {
      rif->read(lut_file);
      std::list<RectificationInfoBlock *> &blocks = rif->blocks();

      printf("File:         %s\n"
	     "Version:      %u\n"
	     "Endianess:    %s\n"
#if __WORDSIZE == 64
	     "Num Blocks:   %u/%lu (header/read)\n"
	     "GUID:         0x%016lX\n"
#else
	     "Num Blocks:   %u/%u (header/read)\n"
	     "GUID:         0x%016llX\n"
#endif
	     "Camera Model: %s\n",
	     lut_file, rif->version(),
	     rif->is_little_endian() ? "little endian" : "big endian",
	     rif->num_blocks(), blocks.size(),
	     rif->guid(), rif->model());

      unsigned int u = 1;
      std::list<RectificationInfoBlock *>::const_iterator i;
      for (i = blocks.begin(); i != blocks.end(); ++i) {
	RectificationInfoBlock *rib = *i;

	const char *type;
	switch (rib->type()) {
	case FIREVISION_RECTINFO_TYPE_LUT_16x16:
	  type = "Rectification LUT (16x16)";
	  break;
	default:
	  type = "Unknown rectification info";
	  break;
	}

	const char *camera;
	switch (rib->camera()) {
	case FIREVISION_RECTINFO_CAMERA_MAIN:
	  camera = "Main";
	  break;
	case FIREVISION_RECTINFO_CAMERA_LEFT:
	  camera = "Left";
	  break;
	case FIREVISION_RECTINFO_CAMERA_RIGHT:
	  camera = "Right";
	  break;
	case FIREVISION_RECTINFO_CAMERA_CENTER:
	  camera = "Center";
	  break;
	case FIREVISION_RECTINFO_CAMERA_TOP:
	  camera = "Top";
	  break;
	default:
	  camera = "Unknown";
	  break;
	}

	printf("\nRectInfo Block No. %u\n"
	       "Type:       %s\n"
	       "Camera:     %s\n"
	       "Size:       %u\n",
	       u++, type, camera, rib->size());

	switch (rib->type()) {
	case FIREVISION_RECTINFO_TYPE_LUT_16x16:
	  {
	    RectificationLutInfoBlock *rlib = dynamic_cast<RectificationLutInfoBlock *>(rib);
	    if ( rlib == NULL ) {
	      printf("** Failure to access LUT_16x16\n");
	    } else {
	      printf("LUT width:  %u\n"
		     "LUT height: %u\n",
		     rlib->pixel_width(), rlib->pixel_height());
	    }
	  }
	  break;
	default:
	  printf("** No additional information available for this info type\n");
	  break;
	}
      }

    } catch (Exception &e) {
      fprintf(stderr, "Failed to read lut file %s\n", lut_file);
      e.print_trace();
    }

    delete rif;

  }
}


int
main(int argc, char **argv)
{

  ArgumentParser argp(argc, argv, "rvid");

  if (argp.num_items() == 0) {
    print_usage(&argp);
  }

  if ( argp.has_arg("r") ) {
    return retrieve(&argp);
  } else if ( argp.has_arg("v") ) {
    return verify(&argp);
  } else if ( argp.has_arg("d") ) {
    return deep_verify(&argp);
  } else if ( argp.has_arg("i") ) {
    print_info(&argp);
  } else {
    print_usage(&argp);
  }

  return 0;
}