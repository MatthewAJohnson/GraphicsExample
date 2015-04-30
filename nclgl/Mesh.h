#pragma once
#include "OGLRenderer.h"

enum MeshBuffer
{
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER,
	TANGENT_BUFFER,INDEX_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();
	static Mesh* GenerateCircle(const float radius, const float height);
	static Mesh* GenerateCylinder();


	void	SetTexture(GLuint tex) {texture = tex;}
	GLuint  GetTexture()		   {return texture;}

	void SetBumpMap(GLuint tex){bumpTexture = tex;}
	GLuint GetBumpMap(){return bumpTexture;}

	void* mapBuffer(MeshBuffer mb);
	void unMapBuffer();

	GLuint numVertices;
	GLuint numIndices;
	unsigned int* indices;

protected:
	void BufferData();
	void GenerateNormals();

	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3 &a, const Vector3 &b,
							const Vector3 &c, const Vector2 &ta,
							const Vector2 &tb, const Vector2 &tc);

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	
	GLuint type;
	GLuint texture;

	Vector2* textureCoords;
	Vector3* vertices;
	Vector3* normals;
	Vector4* colours;

	
	

	Vector3* tangents;
	GLuint bumpTexture;

	

};