
#include <GL/glut.h>
#include <stdio.h>

#include "TriMesh.h"

namespace xglm {

	void BBox::SetPoints(const std::vector<Vec3f>& v)
	{
		this->Valid(false);
		if( v.size()<1 ) return;
		this->Init( v[0] );
		for( unsigned int i=1; i<v.size(); i++ )
			this->Add( v[i] );
		this->Finish();
	}

	void TriMesh::needBBox()
	{
		if( !_bbox.IsValid() )
			_bbox.SetPoints(_vertices);
		if( !_bbox.IsValid() )
			return;
		// a rough approximation
		_bsph._center = (_bbox._min+_bbox._max)*0.5f;
		_bsph._radius = length(_bbox._size)/2;
		_bsph.Valid(true);
	}

	void TriMesh::needVtxAdjFaces()
	{
		const int F = _faces.size();
		const int V = _vertices.size();
		_vtxAdjFaces.resize( V );
		for( int f = 0; f<F; f++ )
		{
			for( int k=0; k<3; k++ )
			{
				int v = _faces[f][k];
				if( v<0 || v>=V )
				{
					printf("Error in face [%d]'s vertex: %d\n", f, v);
					return;
				}
				_vtxAdjFaces[v].push_back(f);
			}
		}
	}

	void TriMesh::needVtxNbrVertices()
	{
		const int F = _faces.size();
		const int V = _vertices.size();

		_vtxNbrVertices.resize( V );
		for( int v=0; v<V; v++ ) 
			_vtxNbrVertices[v].clear();

		for( int f = 0; f<F; f++ )
		{
			for( int k=0; k<3; k++ )
			{
				int a[2], v = _faces[f][k];
				if( v<0 || v>=V )
				{
					printf("Error in face [%d]'s vertex: %d\n", f, v);
					return;
				}
				a[0] = _faces[f][(k+1)%3];
				a[1] = _faces[f][(k+2)%3];
				for( int j=0; j<2; j++ )
				{
					if( _vtxNbrVertices[v].indexOf(a[j])>=0 )
						continue;
					_vtxNbrVertices[v].push_back(a[j]);						
				}
			}
		}
	}



}
