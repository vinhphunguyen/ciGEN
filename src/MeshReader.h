#ifndef MESH_READER_H
#define MESH_READER_H

struct Global;

void                     readGmshMesh 

    ( Global&     globdat,
      const char* fileName );


void                     readNURBSMesh 

    ( Global&     globdat,
      const char* fileName );

void                     readMesh 

   ( Global&     globdat,
     const char* fileName );


void                     readAbaqusMesh 

   ( Global&     globdat,
     const char* fileName );

#endif
