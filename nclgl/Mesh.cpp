#include "Mesh.h"

Mesh::Mesh()
{
	for(int i = 0; i < MAX_BUFFER; ++i)
	{
		bufferObject[i] = 0;
	}

	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	vertices = NULL;
	colours = NULL;
	type = GL_TRIANGLES;

	texture = 0;
	textureCoords = NULL;

	indices = NULL;
	numIndices = 0;

	normals = NULL;

	tangents = NULL;
	bumpTexture = 0;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] vertices;
	delete[] colours;
	delete[] textureCoords;
	delete[] indices;
	delete[] normals;
	delete[] tangents;
	
	glDeleteTextures(1, &bumpTexture);
}

Mesh* Mesh::GenerateTriangle()
{
	Mesh* m = new Mesh();
	m->numVertices = 3;
	
	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f,   0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f,  -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	
	return m;
}

Mesh* Mesh::GenerateQuad()
{
	Mesh* m = new Mesh();
	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Vector4[m->numVertices];
	m->normals = new Vector3[m->numVertices];
	m->tangents = new Vector3[m->numVertices];

	m->vertices[0] = Vector3(-1.0f,-1.0f,0.0f);
	m->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	m->vertices[2] = Vector3(1.0f,-1.0f,0.0f);
	m->vertices[3] = Vector3(1.0f,1.0f,0.0f);

	m->textureCoords[0] = Vector2(0.0f,1.0f);
	m->textureCoords[1] = Vector2(0.0f,0.0f);
	m->textureCoords[2] = Vector2(1.0f,1.0f);
	m->textureCoords[3] = Vector2(1.0f,0.0f);
	
	for(int i = 0; i < 4; ++i)
	{
		m->colours[i] = Vector4(1.0f,1.0f,1.0f,1.0f);
		m->normals[i] = Vector3(0.0f, 0.0f, -1.0f);
		m->tangents[i] = Vector3(1.0f, 0.0f, 0.0f);
	}

	m->BufferData();
	return m;
}

void Mesh::BufferData()
{
	glBindVertexArray(arrayObject);
	
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(VERTEX_BUFFER);

	if(textureCoords)
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	if(colours)
	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
	
	if(indices)
	{
		glGenBuffers(1,&bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLuint),indices,GL_STATIC_DRAW);
	}

	if(normals)
	{
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0,0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}

	if(tangents)
	{
	 glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
	 glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
	 glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), tangents, GL_STATIC_DRAW);
	 glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, 0,0);
	 glEnableVertexAttribArray(TANGENT_BUFFER);
	}

	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(arrayObject);
	//glDrawArrays(type, 0, numVertices);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumpTexture);
	if(bufferObject[INDEX_BUFFER])
	{
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(type, 0, numVertices);
	}
	

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);


}

void* Mesh::mapBuffer(MeshBuffer mb)
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[mb]);

	return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);


}

void Mesh::unMapBuffer()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Mesh::GenerateNormals()
{
	if(!normals)
	{
		normals = new Vector3[numVertices];
	}

	for(GLuint i = 0; i < numVertices; ++i)
	{
		normals[i] = Vector3();
	}

	if(indices)//generate per-vertex normals
	{
		for(GLuint i = 0; i < numIndices; i+=3)
		{
			unsigned int a = indices[i];
			unsigned int b = indices[i+1];
			unsigned int c = indices[i+2];

			Vector3 normal = Vector3::Cross((vertices[b]-vertices[a]),(vertices[c] - vertices[a]));
			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;
		}
	}
	else //it's just a list of triangles, so generate face normals
	{
		for(GLuint i = 0; i < numVertices; i+=3)
		{
			Vector3 &a = vertices[i];
			Vector3 &b = vertices[i+1];
			Vector3 &c = vertices[i+2];

			Vector3 normal = Vector3::Cross(b-a,c-a);

			normals[i] = normal;
			normals[i+1] = normal;
			normals[1+2] = normal;
		}
	}

	for(GLuint i = 0; i < numVertices; ++i)
	{
		normals[i].Normalise();
	}
}

void Mesh::GenerateTangents()
{
	if(!tangents)
	{
		tangents = new Vector3[numVertices];
	}

	for(GLuint i = 0; i < numVertices; ++i)
	{
		tangents[i] = Vector3();
	}

	if(indices)
	{
		for(GLuint i = 0; i < numIndices; i+=3)
		{
			int a = indices[i];
			int b = indices[i+1];
			int c = indices[i+2];

			Vector3 tangent = GenerateTangent(vertices[a], vertices[b], vertices[c], 
											  textureCoords[a], textureCoords[b], textureCoords[c]);
		
			tangents[a] += tangent;
			tangents[b] += tangent;
			tangents[c] += tangent;
		}
	}
	else
	{
		for(GLuint i = 0; i < numVertices; i+=3)
		{
			Vector3 tangent = GenerateTangent(vertices[i], vertices[i+1], vertices[i+2], textureCoords[i],
											  textureCoords[i+1], textureCoords[i+2]);

			tangents[i] += tangent;
			tangents[i+1] += tangent;
			tangents[i+2] += tangent;
		}
	}

	for(GLuint i = 0; i < numVertices; ++i)
	{
		tangents[i].Normalise();
	}

}

Vector3 Mesh::GenerateTangent(const Vector3 &a, const Vector3 &b, const Vector3 &c, 
							  const Vector2 &ta, const Vector2 &tb, const Vector2 &tc)
{
	Vector2 coord1 = tb-ta;
	Vector2 coord2 = tc-ta;
	
	Vector3 vertex1 = b-a;
	Vector3 vertex2 = c-a;

	Vector3 axis = Vector3(vertex1 * coord2.y - vertex2 * coord1.y);
	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}



 Mesh* Mesh::GenerateCircle(const float radius, const float height)
{
	Mesh* m = new Mesh();
	const int stepCount = 50;
	const float divisor = 360.0f / stepCount;
	
	m->type = GL_TRIANGLES;

	m->numVertices = stepCount*2*3;

	m->vertices = new Vector3[m->numVertices];
	
	m->textureCoords = new Vector2[m->numVertices];

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.5f, 0.25f, 1.0f);
	
	std::vector<unsigned int> my_indices;

	m->vertices[0] = Vector3(0,0,0);
	m->vertices[1] = Vector3(0,height,0);
	m->textureCoords[0] = Vector2(0.5f,0.5f);
	m->textureCoords[1] = Vector2(0.5f, 0.5f);
	unsigned int index = 0;

	//Circle Top
	{
		for(int i = 0; i < (stepCount ); ++i)
		{
			float startx = radius * (float)cos(DegToRad(i * divisor));
			float starty = radius * (float)sin(DegToRad(i * divisor));
			
			float texCoordX = i / (float)(stepCount)*2;
			
			if(texCoordX > 1)
			{
				texCoordX = 2.0f - texCoordX;
			}
			m->vertices[i+2] = Vector3(startx, 0, starty);	
			m->colours[i+2] = Vector4(0.5f, 0.0f, 0.5f,1.0f);
			m->textureCoords[i+2] = Vector2(texCoordX*4,0);

			if (i > 0)
			{
				my_indices.push_back(i + 1);
				my_indices.push_back(0);			
				my_indices.push_back(i + 2);
			}

			
		}
		my_indices.push_back(0);
		my_indices.push_back(2);		
		my_indices.push_back(stepCount+1);
	}

	//Circle Bottom
	{
		for(int i = 0; i < (stepCount ); ++i)
		{
			float startx = radius * (float)cos(DegToRad(i * divisor));
			float starty = radius * (float)sin(DegToRad(i * divisor));
			
			float texCoordX = i / (float)(stepCount)*2;
			
			if(texCoordX > 1)
			{
				texCoordX = 2.0f - texCoordX;
			}
			
			m->vertices[stepCount + i+2] = Vector3(startx, height, starty);	
			m->colours[stepCount + i+2] = Vector4(0.5f, 0.0f, 0.5f,1.0f);
			m->textureCoords[stepCount + i+2] = Vector2(texCoordX*4,1);

			if (i > 0)
			{
				my_indices.push_back(1);
				my_indices.push_back(stepCount + i + 1);
				my_indices.push_back(stepCount + i + 2);
			}

		
		}
		my_indices.push_back(stepCount + 2);
		my_indices.push_back(1);		
		my_indices.push_back(stepCount + stepCount+1);
	}

	for(int i = 0; i < (stepCount ); ++i)
	{
		unsigned int a, b, c, d;
		a = i + 2;
		b = i + 3;
		c = stepCount + i + 2;
		d = stepCount + i + 3;

		//Wrap around vertices when a full circle has been completed
		if (i == stepCount - 1)
		{
			d = stepCount + 2;
			b = 2;
		}

		my_indices.push_back(c);
		my_indices.push_back(b);
		my_indices.push_back(d);
		
		my_indices.push_back(c);
		my_indices.push_back(a);	
		my_indices.push_back(b);
	}


	m->numIndices = my_indices.size();
	m->indices = new unsigned int[m->numIndices];
	memcpy(m->indices, &my_indices[0], m->numIndices * sizeof(unsigned int));
	
	//think this might not work?
	m->GenerateNormals();
	m->GenerateTangents();

	m->BufferData();
	return m;

//	m->GenerateNormals();
	//m->GenerateTangents();
	//for(int x = 1; x < stepCount; ++x)
	//{
	//	counter++;
	//	for(int i = stepCount*2; i < (stepCount * 4); ++i)
	//	{
	//		m->vertices[i*3] = Vector3(m->vertices[x*3].x, m->vertices[x*3].y, 0);
	//		m->vertices[(i*3)+1] = Vector3(m->vertices[((x+stepCount) * 3)].x, m->vertices[((x+stepCount) * 3)].y,50);
	//		m->vertices[(i*3)+2] = Vector3(m->vertices[((x+stepCount+1) * 3)].x, m->vertices[((x+stepCount+1) * 3)].y, 50);

	//		/*m->vertices[i*3+3] = Vector3(m->vertices[x + stepCount].x, m->vertices[x + stepCount].y, 50);
	//		m->vertices[(i*3)+4] = Vector3(m->vertices[x+1].x, m->vertices[x+1].y,50);
	//		m->vertices[(i*3)+5] = Vector3(m->vertices[x].x, m->vertices[x].y, 0);*/

	//		m->colours[(i*3)] = Vector4(0.5f, 1.0f, 1.5f,1.0f);
	//		m->colours[(i*3)+1] = Vector4( 0.0f, 1.5f, 1.5f,1.0f);
	//		m->colours[(i*3)+2] = Vector4( 0.0f, 1.5f, 1.5f,1.0f);
	//	counter++;
	//	}
	

	//counter = 0;
	//for(int x = 1; x < stepCount; ++x)
	//{
	//	counter++;
	//	for(int i = stepCount*2; i < (stepCount * 4); ++i)
	//	{
	//		m->vertices[i*3] = Vector3(m->vertices[x*3].x, m->vertices[x*3].y, 0);
	//		m->vertices[(i*3)+1] = Vector3(m->vertices[((x+stepCount) * 3)].x, m->vertices[((x+stepCount) * 3)].y,50);
	//		m->vertices[(i*3)+2] = Vector3(m->vertices[((x+stepCount+1) * 3)].x, m->vertices[((x+stepCount+1) * 3)].y, 50);

	//		/*m->vertices[i*3+3] = Vector3(m->vertices[x + stepCount].x, m->vertices[x + stepCount].y, 50);
	//		m->vertices[(i*3)+4] = Vector3(m->vertices[x+1].x, m->vertices[x+1].y,50);
	//		m->vertices[(i*3)+5] = Vector3(m->vertices[x].x, m->vertices[x].y, 0);*/

	//		m->colours[(i*3)] = Vector4(0.5f, 1.0f, 1.5f,1.0f);
	//		m->colours[(i*3)+1] = Vector4( 0.0f, 1.5f, 1.5f,1.0f);
	//		m->colours[(i*3)+2] = Vector4( 0.0f, 1.5f, 1.5f,1.0f);
	//	counter++;
	//	}
	//

	/*;
	int counter = 0;
	for(int i = stepCount; i < stepCount * 2; ++i)
	{
			//	if(i >= stepCount+2){break;}
		float startx = radius * (float)cos(DegToRad(counter * divisor));
		float starty = radius * (float)sin(DegToRad(counter * divisor));
	
		float startx2 = radius * (float)cos(DegToRad((counter+1) * divisor));
		float starty2 = radius * (float)sin(DegToRad((counter+1) * divisor));
		
		
		m->vertices[(i*3)+1] = Vector3(startx, starty, 50);
		m->vertices[(i*3)+2] = Vector3(startx2,starty2,50);
		
		m->colours[(i*3)] = Vector4(0.5f, 1.0f, 1.5f,1.0f);
		m->colours[(i*3)+1] = Vector4( 0.0f, 1.5f, 0.5f,1.0f);
		m->colours[(i*3)+2] = Vector4( 0.0f, 0.5f, 0.5f,1.0f);

		my_indices.push_back((i*3));
		my_indices.push_back((i*3)+1);
		my_indices.push_back((i*3)+2);

		counter++;
		count = i * 3;
	}
	*/
	//unsigned int index = 0;
	//for(int i = 0; i < (stepCount ); ++i)
	//{
	//	//	float endy = radius * (float) cos(DegToRad((i+1) * divisor)) + at.x;
	//	//float endx = radius * (float)sin(DegToRad((i+2) * divisor)) + at.y;
	//	//if(i >= 6){break;}
	//	float startx = radius * (float)cos(DegToRad(i * divisor));
	//	float starty = radius * (float)sin(DegToRad(i * divisor));
	//
	//	float startx2 = radius * (float)cos(DegToRad((i+1 )* divisor));
	//	float starty2 = radius * (float)sin(DegToRad((i+1) * divisor));
	//	
	//	m->vertices[i*3] = Vector3(0,0,0);
	//	m->vertices[(i*3)+1] = Vector3(startx, starty, 0);
	//	m->vertices[(i*3)+2] = Vector3(startx2,starty2,0);
	//	
	//	m->colours[(i*3)] = Vector4(0.5f, 0.0f, 0.5f,1.0f);
	//	m->colours[(i*3)+1] = Vector4( 1.0f, 0.5f, 0.5f,1.0f);
	//	m->colours[(i*3)+2] = Vector4( 1.0f, 0.5f, 0.5f,1.0f);

	//	my_indices.push_back((i*3));
	//	my_indices.push_back((i*3)+1);
	//	my_indices.push_back((i*3)+2);

	//	count=i*3;

	//}

	//;
	//int counter = 0;
	//for(int i = stepCount; i < stepCount * 2; ++i)
	//{
	//		//	if(i >= stepCount+2){break;}
	//	float startx = radius * (float)cos(DegToRad(counter * divisor));
	//	float starty = radius * (float)sin(DegToRad(counter * divisor));
	//
	//	float startx2 = radius * (float)cos(DegToRad((counter+1) * divisor));
	//	float starty2 = radius * (float)sin(DegToRad((counter+1) * divisor));
	//	
	//	
	//	m->vertices[(i*3)+1] = Vector3(startx, starty, 50);
	//	m->vertices[(i*3)+2] = Vector3(startx2,starty2,50);
	//	
	//	m->colours[(i*3)] = Vector4(0.5f, 1.0f, 1.5f,1.0f);
	//	m->colours[(i*3)+1] = Vector4( 0.0f, 1.5f, 0.5f,1.0f);
	//	m->colours[(i*3)+2] = Vector4( 0.0f, 0.5f, 0.5f,1.0f);

	//	my_indices.push_back((i*3));
	//	my_indices.push_back((i*3)+1);
	//	my_indices.push_back((i*3)+2);

	//	counter++;
	//	count = i * 3;
	//}

	/*m->vertices[109] = Vector3(m->vertices[1].x,m->vertices[1].y, 0 );
	m->vertices[110] = Vector3(m->vertices[55].x, m->vertices[55].y, 50);
	m->vertices[111] = Vector3(m->vertices[56].x, m->vertices[56].y, 50);

	m->colours[109] = Vector4(0.5f, 1.0f, 1.5f,1.0f);
	m->colours[110] = Vector4( 0.0f, 1.0f, 0.5f,1.0f);
	m->colours[111] = Vector4( 0.0f, 1.0f, 0.5f,1.0f);*/

	
 }

static Mesh* GenerateCylinder()
{
	
}