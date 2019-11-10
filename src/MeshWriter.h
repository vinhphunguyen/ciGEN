#ifndef MESH_WRITER_H
#define MESH_WRITER_H

struct Global;

void                     writeJemMesh 

    ( Global&     globdat,
      const char* fileName );

void                     writeAbaqusMesh 

    ( Global&     globdat,
      const char* fileName );

void                     writeMatlabMesh 

    ( Global&     globdat,
      const char* fileName );

void                     writeMesh 

   ( Global&     globdat,
     const char* fileName );


#endif
