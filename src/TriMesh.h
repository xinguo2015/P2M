#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <memory.h>

#include "Math3D/math.h"

namespace xglm {

	class TriFace 
	{
		int _v[3];

	public:
		TriFace()									{}
		TriFace( int v0, int v1, int v2 )			{ V3_MAKE(_v,v0,v1,v2); }
		TriFace( int *v ) 							{ V3_COPY(_v,v); }

		int & operator[] (int i)					{ return _v[i]; }
		int operator[] (int i) const				{ return _v[i]; }
		operator const int * () const				{ return &(_v[0]); }
		operator int * ()							{ return &(_v[0]); }

		int indexOf(int v) const					{ return ( v==_v[0] ? 0 : ( v==_v[1] ? 1 : ( v==_v[2]? 2 : -1 ) ) ); };
		int nextOf(int v) const						{ return ( v==_v[0] ? _v[1] : ( v==_v[1] ? _v[2] : ( v==_v[2]? _v[0] : -1 ) ) ); };
		int prevOf(int v) const						{ return ( v==_v[0] ? _v[2] : ( v==_v[1] ? _v[0] : ( v==_v[2]? _v[1] : -1 ) ) ); };
	};

	class RingNbr : public std::vector<int>
	{
	public:
		RingNbr()                                   { }
		RingNbr( int v0, int v1, int v2 ) 			{ push_back(v0);  push_back(v1); push_back(v2); }
		RingNbr( int v0, int v1, int v2, int v3)	{ push_back(v0);  push_back(v1); push_back(v2); push_back(v3); }
		RingNbr( int *v, int N )					{ resize(N); memcpy( &(front()), v, sizeof(int)*N); }

		int indexOf(int v) const					
		{	
			std::vector<int>::const_iterator f = std::find(begin(),end(), v);
			if( f == end() ) return (-1);
			return f-begin();
		}
		int nextOf(int v) const						
		{
			std::vector<int>::const_iterator f = std::find(begin(),end(), v);
			if( f == end() ) return (-1);
			if( ++f == end() ) return 0;
			return (f-begin());
		}
		int prevOf(int v) const
		{
			std::vector<int>::const_iterator f = std::find(begin(),end(), v);
			if( f == end() ) return (-1);
			if( f == begin() ) return size()-1;
			return (--f-begin());
		}

	};

	// bounding sphere
	// axis aligned bounding box
	class BBox
	{
	public:
		Vec3f _min;
		Vec3f _max;
		Vec3f _size;
		bool  _valid;
	public:
		BBox()								{ _valid = false; }
		void Valid(bool flag)				{ _valid = flag; }
		bool IsValid() const				{ return _valid; }

		void Init(const Vec3f & p)          { _min = _max = p; }
		void Add(const Vec3f & p)			{ _min = min( _min,p ); _max = max( _max,p); }
		void Finish()						{ _size = _max - _min; _valid = true; }
		// calc bounding box
		void SetPoints(const std::vector<Vec3f>& v);
	};

	class BSphere 
	{
	public:
		Vec3f _center;
		float _radius;
		bool  _valid;
	public:
		BSphere()							{ _valid = false; }
		void Valid(bool flag)				{ _valid = flag; }
		bool IsValid() const				{ return _valid; }
	};

	class TriMesh 
	{
	public:

	public:
		// The basics: vertices and faces
		std::vector<Vec3f>   _vertices;
		std::vector<TriFace> _faces;

		// per-vertex properties
		std::vector<Vec3f> _vNormals;

		// per face properties
		std::vector<Vec3f> _fNormals;
		std::vector<float> _fAreas;

		// Connectivity structures:
		//  For each vertex, all neighboring vertices
		std::vector<RingNbr> _vtxNbrVertices;
		//  For each vertex, all neighboring faces
		std::vector<RingNbr> _vtxAdjFaces;
		//  For each face, the three opposite faces attached to its edges
		std::vector<TriFace> _oppFaces;

		// global geometry
		BBox	_bbox; // bounding box
		BSphere	_bsph; // bounding sphere

		// Delete everything
		void clear()
		{
			_vertices.clear();
			_faces.clear();
			_vNormals.clear();
			_fNormals.clear();
		}

		void needVtxNbrVertices();
		void needVtxAdjFaces();
		
		//
		// Useful queries
		//

		// Is vertex v on the mesh boundary?
		bool atBoundary(int v)
		{
			if (_vtxNbrVertices.empty() )
				needVtxNbrVertices();
			if (_vtxAdjFaces.empty())
				needVtxAdjFaces();
			return( _vtxNbrVertices[v].size() != _vtxAdjFaces[v].size() );
		}

		// Normal of face f
		Vec3f calcFaceNormal(int f) const
		{	const int * v = (const int*)_faces[f];
			return cross(
				_vertices[v[1]]-_vertices[v[0]],
				_vertices[v[2]]-_vertices[v[0]]);
		}

		void needFaceNormals( bool needFaceAreas = true)
		{
			int N = _faces.size();
			_fNormals.resize( N );
			if( needFaceAreas )
				_fAreas.resize( N );
			for( int k=0; k<N; k++ ) 
			{	
				_fNormals[k] = calcFaceNormal(k);
				float len = length(_fNormals[k]);
				if( needFaceAreas )
					_fAreas[k] = len*0.5f;
				_fNormals[k] /= len;	
			}
		}
		void needVtxNormals()
		{
			int Fn = _faces.size();
			int Vn = _vertices.size();
			_vNormals.resize( Vn );
			_vNormals.assign( Vn, Vec3f(0,0,0) );
			for( int f=0; f<Fn; f++ ) 
			{	
				Vec3f nml = calcFaceNormal(f);
				for( int k=0; k<3; k++ )
					_vNormals[_faces[f][k]] += nml;
			}
			for( int k=0; k<Vn; k++ )
				_vNormals[k].normalize();
		}

		void needBBox();
	};
}
