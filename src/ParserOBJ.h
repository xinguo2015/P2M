#include <stdio.h>
#include <string.h>

#ifdef _WIN
#pragma warning(disable:4996)
#endif

#ifndef V3_MAKE
#define V3_MAKE(V, x, y, z) \
        ((V)[0] = x,\
         (V)[1] = y,\
         (V)[2] = z)
#endif

#define MaxOBJFace 32
#define MaxOBJLine 2048
#define OneOf2(c,a,b) ((c)==(a)||(c)==(b))

#define ErrReturn(a, b)  do { printf(a); return(b); } while(0)
#define CheckMsg(a,b)  do { if( !a ) printf(b); } while(0)

namespace xglm {

	// a class for parsing a line of OBJ file
	class OBJParser
	{
	public:
		enum EDataType
		{
			DataInvalid,
			DataEmpty,   // empty line
			DataComment, // #
			DataVertex,  // v
			DataFace,    // f
			DataTexcoord,// vt
			DataNormal,  // vn
		};

		// a pointer to the string to parse
		char * _ptr;
		// data type of the line
		EDataType _datatype;
		// result data
		float		_coord[16];				// coordinate
		int			_dim;					// coord dimension
		int			_face[3][MaxOBJFace];	// index for the face
		int			_vertNum;				// vertex number
		int			_hasTexc;				// has texture coord index?
		int			_hasNormal;				// has normal index?
		int			_tris[3][MaxOBJFace*3]; // index for the triangulated face

	public:
		static char * GetLine(char *buf, int maxline, FILE* fp)
		{
			if( fgets(buf, maxline, fp) ) return buf;
			return NULL;
		}

		bool ParseLine(char *buf)
		{
			_ptr = buf;
			ParseLineType();

			switch ( _datatype ) 
			{
			case DataFace:
				return ParseFace();
			case DataVertex:   
			case DataNormal:   
			case DataTexcoord: 
				return ParseCoord();
			default:
				break;
			}

			return true;
		}

		int TriangulateFace()
		{	
			int first = 0, last = _vertNum-1;
			int flag = 1, R = 0;
			while( first+1<last )
			{
				int vMiddle = flag ? first+1 : last-1;
				V3_MAKE(                  _tris[0]+R, _face[0][first],  _face[0][vMiddle],  _face[0][last]);
				if( _hasTexc )	 V3_MAKE( _tris[1]+R, _face[1][first],  _face[1][vMiddle],  _face[1][last]);
				if( _hasNormal ) V3_MAKE( _tris[2]+R, _face[2][first],  _face[2][vMiddle],  _face[2][last]);
				if( flag ) first ++; else last --;
				flag = !flag;
				R += 3;
			}
			return _vertNum-2;
		}

	private:
		EDataType ParseLineType()
		{
			while( OneOf2( _ptr[0], '\t', ' ') )
				_ptr++;

			if( OneOf2(_ptr[0],'f','F') && OneOf2(_ptr[1],' ','\t'))
			{	// f
				_ptr += 2; _datatype = DataFace;
			}
			else if( OneOf2(_ptr[0],'v','V') && OneOf2(_ptr[1],' ','\t') )
			{	// v
				_ptr += 2; _datatype = DataVertex;
			}
			else if( OneOf2(_ptr[0],'v','V') && OneOf2(_ptr[1],'t','T') && OneOf2(_ptr[2],' ','\t'))
			{	// vt
				_ptr += 3; _datatype = DataTexcoord;
			}
			else if( OneOf2(_ptr[0],'v','V') && OneOf2(_ptr[1],'n','N') && OneOf2(_ptr[2],' ','\t'))
			{	// vn
				_ptr += 3; _datatype = DataNormal;
			}
			else if( _ptr[0]=='#' )
			{
				_ptr += 1; _datatype = DataComment;
			}
			else if( OneOf2(_ptr[0], '\n', '\0' ) )
				_datatype = DataEmpty;
			else
				_datatype = DataInvalid;
			return _datatype;
		}
		bool ParseCoord()
		{
			int cRead = 0; _dim = 0; 
			while( sscanf(_ptr, "%f%n", _coord+_dim, &cRead)==1 )  
			{
				_dim++; _ptr += cRead; 
			}
			return ( _dim>0 );
		}
		bool ParseFace()
		{
			int ff, cRead, finished;

			if( (ff = FaceFormat())==0 )
				ErrReturn("Error: wrong format\n", false);

			_vertNum = finished = 0;
			while( ! finished )
			{
				if( (ff==100 && sscanf(_ptr, "%d%n", _face[0]+_vertNum, &cRead)==1) ||
					(ff==110 && sscanf(_ptr, "%d/%d%n", _face[0]+_vertNum, _face[1]+_vertNum, &cRead)==2) ||
					(ff==101 && sscanf(_ptr, "%d//%d%n", _face[0]+_vertNum, _face[2]+_vertNum, &cRead)==2)   ||
					(ff==111 && sscanf(_ptr, "%d/%d/%d%n",_face[0]+_vertNum, _face[1]+_vertNum, _face[2]+_vertNum, &cRead)==3) )
				{
					_vertNum++;
					_ptr += cRead;
				}
				else
					finished = 1;
				if( _vertNum==MaxOBJFace )
					ErrReturn("Error: maximum face size reached\n", false);
			}
			if( _vertNum<3 )
				ErrReturn("Error: degenerated face found\n", false);
			return true;
		}

		int FaceFormat()
		{
			int ff = 0;
			char * p = strchr(_ptr,'/');
			if( ! p ) 
				ff = 100; // v only
			else if( p[1]=='/' ) 
				ff = 101; // v//n;
			else 
			{
				// there are possible twoes : v/t or v/n/t
				while( ff==0 && *p++!='\0' )
				{
					if( OneOf2(p[0],' ', '\t') ) 
						ff = 110; // v/t
					else if( *p=='/' )
						ff = 111; // v/t/n
				}
			}
			if( ff==0 ) // v/t, but only one vertex in the string
				ff = 110; 
			_hasNormal = ff%10;
			_hasTexc = ff/10%10;
			return ff;
		}
	};

};
