
#include <GL/glut.h>

#include "TriMesh.h"
#include "MeshRender.h"

namespace xglm {


	template<>
	void MeshRender<TriMesh>::draw( TriMesh * mesh, const RenderOption& ops )
	{
		// vertices
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(GL_FLOAT)*3, &mesh->_vertices[0][0]);
		// normals
		if( mesh->_vNormals.size()>0 )
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, sizeof(GL_FLOAT)*3, &mesh->_vNormals[0][0]);
		}
		// draw triangles
		glDrawElements(GL_TRIANGLES, mesh->_faces.size()*3, GL_UNSIGNED_INT, &mesh->_faces[0][0]);
		// draw points
		if( ops.getOption(RenderOption::Points) )
		{
			// draw as points
			glPointSize( ops.getPointSize() );
			glDrawArrays(GL_POINTS, 0, mesh->_vertices.size());
		}
		// bounding box
		if( ops.getOption(RenderOption::BBox) )
		{
			Vec3f c = mesh->_bbox._min + mesh->_bbox._max; c *= 0.5f;
			Vec3f & s = mesh->_bbox._size;
			glColor4f(1.0f, 0.0f, 1.0f, 0.3f);
			glTranslatef(c[0], c[1], c[2]);
			glScalef(s[0], s[1], s[2]);
			glutSolidCube(1.0);
		}
	}



}
