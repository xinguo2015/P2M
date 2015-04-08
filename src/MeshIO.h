#ifndef _____mesh_io_h_____________
#define _____mesh_io_h_____________

#include <stdio.h>
#include <string>

#include "ParserOBJ.h"


namespace xglm 
{
	using std::string;

	template<typename TMesh>
	class MeshIO
	{
	public:
		static bool createFromFile(TMesh* mesh,  string fname );
		static bool createFromOBJ(TMesh* mesh,  FILE* fp );
		static bool createFromPLY(TMesh* mesh,  FILE* fp );
	};

}

#endif
