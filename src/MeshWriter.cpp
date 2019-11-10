#include <boost/algorithm/string.hpp>


#include "MeshWriter.h"
#include "Global.h"


void                     writeMesh 

   ( Global&     globdat,
     const char* fileName )

{
  string    filename  ( fileName );
  StrVector filenames;

  boost::split ( filenames, filename, boost::is_any_of(".") );

  if      ( filenames[1] == "mesh" )
  {
    writeJemMesh ( globdat, fileName );
  }
  else if ( filenames[1] == "inp" )
  {
    writeAbaqusMesh ( globdat, fileName );
  }
  else
  {
    cout << "output mesh format is not yet supported!!!\n";
    exit(1);
  }

  // if Matlab mesh is needed

  if ( globdat.isMatlab ) writeMatlabMesh ( globdat, fileName );
}


