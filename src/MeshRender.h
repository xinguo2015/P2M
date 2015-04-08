#ifndef _____mesh_render_h_____________
#define _____mesh_render_h_____________

namespace xglm {

	class RenderOption
	{
	public:
		enum DrawElem
		{
			Points,
			Edges,
			Faces,
			Normals,
			BBox,
			// the count of items
			ElemCount
		};

		bool    _options[ElemCount];
		float   _pointSize;

	public:
		RenderOption()
		{
			for( int i=0; i<ElemCount; i++ ) 
				_options[i] = false;
			_options[Faces] = true;
			_pointSize = 2.f;
		}

		bool  getOption(int elemID) const	{ return _options[elemID]; }
		bool& getOption(int elemID) 		{ return _options[elemID]; }
		float getPointSize() const			{ return _pointSize; }	
	};

	template<typename TMesh>
	class MeshRender
	{
	public:
		static void draw(TMesh* mesh, const RenderOption & ops);
		static void build_tstrip(TMesh* pObj);
	};

}

#endif
