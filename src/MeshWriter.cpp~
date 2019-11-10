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

  if ( filenames[1] == "mesh" )
  {
    writeJemMesh ( globdat, fileName );
  }
  if ( filenames[1] == "inp" )
  {
    writeAbaqusMesh ( globdat, fileName );
  }
  else
  {
    cout << "not yet supported!!!\n";
    exit(1);
  }
}


