/*
 *
 *    Author:  Xinguo Liu, Zhejiang University
 *     Email:  xgliu@cad.zju.edu.cn
 *
 */

#include <vector>
#include <string>
#include <string.h>


#include "TriMesh.h"
#include "MeshIO.h"


namespace xglm {

	int loadOBJ(FILE* fp, std::vector<Vec3f> & vertices, std::vector<TriFace> & faces);
	//int loadPLY(FILE* fp, std::vector<Vec3f> & vertices, std::vector<TriFace> & faces);

	template<>
	bool MeshIO<TriMesh>::createFromOBJ(TriMesh * mesh,  FILE * fp )
	{
		if( !loadOBJ(fp, mesh->_vertices, mesh->_faces) )
			return false;
		mesh->needBBox();
		mesh->needFaceNormals();
		mesh->needVtxNormals();
		mesh->needVtxAdjFaces();
		mesh->needVtxNbrVertices();
		return true;
	}

	template<>
	bool MeshIO<TriMesh>::createFromPLY(TriMesh * mesh,  FILE * fp )
	{
	//	if( !loadPLY(fp, mesh->_vertices, mesh->_faces) )
			return false;
		mesh->needBBox();
		mesh->needFaceNormals();
		mesh->needVtxNormals();
		mesh->needVtxAdjFaces();
		mesh->needVtxNbrVertices();
		return true;
	}

	template<>
	bool MeshIO<TriMesh>::createFromFile(TriMesh * mesh, std::string fname)
	{
		const char * pext = fname.length() ? strrchr(fname.c_str(),'.') : NULL;
		if( ! pext ) return false;

        FILE *fp = fopen(fname.c_str(),"rt");
		if( ! fp ) return false;

		bool bResult = false;
		if( strcasecmp(pext,".obj")==0 )
            bResult = MeshIO<TriMesh>::createFromOBJ(mesh, fp);
        else if( strcasecmp(pext,".ply")==0 )
            bResult = MeshIO<TriMesh>::createFromPLY(mesh, fp);
		fclose(fp);
		return bResult;
	}

}
