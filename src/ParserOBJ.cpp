
#include "TriMesh.h"
#include "ParserOBJ.h"

namespace xglm {
	
	// Read an obj file
	int loadOBJ(FILE* fp, 
		std::vector<Vec3f> & vertices,
		std::vector<TriFace> & faces)
	{
		std::vector<char> linebuffer(MaxOBJLine);
		char *pbuf = &linebuffer[0];
		OBJParser p;

		vertices.clear();
		faces.clear();

		while( p.GetLine(pbuf, MaxOBJLine, fp) )
		{
			if( ! p.ParseLine(pbuf) )
				return 0; // error occurs
			switch( p._datatype )
			{
			case OBJParser::DataFace :
				p.TriangulateFace();
				for( int k=0; k<p._vertNum-2; k++ )
					faces.push_back(TriFace(p._tris[0][k*3]-1, p._tris[0][k*3+1]-1, p._tris[0][k*3+2]-1));
				break;
			case OBJParser::DataVertex :
				vertices.push_back( Vec3f(p._coord) );
				break;
			default:
				break;
			}
		}

		return 1;
	}	
	
	
};
