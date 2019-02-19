#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//creating rotation matrix
	glm::mat4 rotationMatrix(1);
	//generating angle of rotation
	float angleOfRotation = (2 * PI) / a_nSubdivisions;
	rotationMatrix = glm::rotate(rotationMatrix, angleOfRotation, vector3(0, 0, a_fRadius));
	//defining start points
	vector3 startingPoint = vector3(0, 0, 0);
	vector3 startingPoint2 = vector3(a_fRadius, 0, 0);
	vector3 startingPoint3 = (vector3)(rotationMatrix * vector4(startingPoint2, 0));

	//makes base bit
	for (uint i = 0; i < a_nSubdivisions; i++) {
		//calculates new starting points
		startingPoint2 = (vector3)(rotationMatrix * vector4(startingPoint2, 0));
		startingPoint3 = (vector3)(rotationMatrix * vector4(startingPoint2, 0));

		//displays to screen
		AddTri(startingPoint, startingPoint2, startingPoint3);
		
	}

	//makes cone bit
	startingPoint = vector3(0, 0, (a_fHeight*(-1)));
	for (uint i = 0; i < a_nSubdivisions; i++) {
		//calculates new starting points
		startingPoint2 = (vector3)(rotationMatrix * vector4(startingPoint2, 0));
		startingPoint3 = (vector3)(rotationMatrix * vector4(startingPoint2, 0));

		//displays to screen
		AddTri(startingPoint3, startingPoint2, startingPoint);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//creating rotation matrix
	glm::mat4 rotationMatrix(1);
	//generating angle of rotation
	float angleOfRotation = (2 * PI) / a_nSubdivisions;
	rotationMatrix = glm::rotate(rotationMatrix, angleOfRotation, vector3(0, 0, 1));
	//defining start points
	vector3 startingPoint = vector3(0, 0, 0);
	vector3 startingPoint2 = vector3(a_fRadius, 0, 0);
	vector3 startingPoint3 = (vector3)(rotationMatrix * vector4(startingPoint2, 0));

	//makes cylinder
	for (uint i = 0; i < a_nSubdivisions; i++) {
		//calculates new starting points
		startingPoint2 = (vector3)(rotationMatrix * vector4(startingPoint2, 0));
		startingPoint3 = (vector3)(rotationMatrix * vector4(startingPoint2, 0));

		//displays to screen
		//makes bottom
		AddTri(startingPoint3, startingPoint2, startingPoint);
		//makes sides
		AddQuad(startingPoint2, startingPoint3, vector3(startingPoint2.x, startingPoint2.y, startingPoint2.z + a_fHeight), vector3(startingPoint3.x, startingPoint3.y, startingPoint3.z + a_fHeight));
		//makes top
		AddTri(vector3(startingPoint.x, startingPoint.y, startingPoint.z + a_fHeight), vector3(startingPoint2.x, startingPoint2.y, startingPoint2.z + a_fHeight), vector3(startingPoint3.x, startingPoint3.y, startingPoint3.z + a_fHeight));
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//creating rotation matrix
	glm::mat4 rotationMatrix(1);
	//generating angle of rotation
	float angleOfRotation = (2 * PI) / a_nSubdivisions;
	rotationMatrix = glm::rotate(rotationMatrix, angleOfRotation, vector3(0, 0, 1));
	//defining start points
	vector3 startingPoint = vector3(0, 0, 0);
	
	vector3 startingPoint1 = vector3(a_fOuterRadius, 0, 0);
	vector3 startingPoint2 = (vector3)(rotationMatrix * vector4(startingPoint1, 0));

	vector3 startingPoint3 = vector3(a_fInnerRadius, 0, 0);
	vector3 startingPoint4 = (vector3)(rotationMatrix * vector4(startingPoint3, 0));

	//makes tube1
	for (uint i = 0; i < a_nSubdivisions; i++) {
		//calculates new starting points
		startingPoint1 = (vector3)(rotationMatrix * vector4(startingPoint1, 0));
		startingPoint2 = (vector3)(rotationMatrix * vector4(startingPoint1, 0));
		startingPoint3 = (vector3)(rotationMatrix * vector4(startingPoint3, 0));
		startingPoint4 = (vector3)(rotationMatrix * vector4(startingPoint3, 0));

		//displays to screen
		//makes bottom
		AddQuad(startingPoint2, startingPoint1, startingPoint4, startingPoint3);

		//makes outer siding
		AddQuad(startingPoint1, startingPoint2, vector3(startingPoint1.x, startingPoint1.y, startingPoint1.z + a_fHeight), vector3(startingPoint2.x, startingPoint2.y, startingPoint2.z + a_fHeight));
		
		//makes inner siding
		AddQuad(vector3(startingPoint3.x, startingPoint3.y, startingPoint3.z + a_fHeight), vector3(startingPoint4.x, startingPoint4.y, startingPoint4.z + a_fHeight), startingPoint3, startingPoint4);
		
		//makes top
		AddQuad(vector3(startingPoint1.x, startingPoint1.y, startingPoint1.z + a_fHeight), vector3(startingPoint2.x, startingPoint2.y, startingPoint2.z + a_fHeight), vector3(startingPoint3.x, startingPoint3.y, startingPoint3.z + a_fHeight), vector3(startingPoint4.x, startingPoint4.y, startingPoint4.z + a_fHeight));
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	//Online resource used to guide the creation of this function : http://www.songho.ca/opengl/gl_sphere.html

	//variable declaration
	//to hold x, y, and z vertice positions
	float posX, posY, posZ, xyCalc;
	float horizontalAngle, zAngle;

	//vector to hold the sphere vertices generated in the first loop
	std::vector<vector3> m_vertices;

	//calculating verticies
	for (int i = 0; i <= a_nSubdivisions; i++) {

		//calculating the zAngle (or stackAngle) for the sphere
		//this is the angle at the 'top' of the sphere, that the melong shaped slices of the sphere are made of
		zAngle = PI / 2 - i * (PI / a_nSubdivisions);
		//calculating the distance of xy to be used in future calculations
		xyCalc = a_fRadius * cosf(zAngle);
		//Since the nested for loop does one cyclinder-like loop at a time this position only needs to be changed as the vertical position is looped through
		posZ = a_fRadius * sinf(zAngle);

		//Does the horizontal vertex calculation
		for (int j = 0; j <= a_nSubdivisions; j++) {

			//calculating the horizontal angle (or sectorAngle) that determines the horizontal rotation of a given triangle
			horizontalAngle = j * (2 * PI / a_nSubdivisions);

			//calculating the x and y position based on the xy calculation
			posX = xyCalc * cosf(horizontalAngle);
			posY = xyCalc * sinf(horizontalAngle);

			//adding the vertice to the vector
			m_vertices.push_back(vector3(posX, posY, posZ));
			
		}
	}

	//index of point 1 and 2 for triangle drawing purposes
	int point1, point2;

	//drawing tris by calling AddTri with the vector of vertices
	for (int i = 0; i < a_nSubdivisions; i++) {
		
		//calculating what point in the vector of vertices should be used for point 1
		point1 = i * (a_nSubdivisions + 1);
		//incrementing point1 by number of sub divisions +1 as the second angle in the triangle
		point2 = point1 + a_nSubdivisions + 1;

		//looping through and drawing triangles
		for (int j = 0; j < a_nSubdivisions; j++) {
			
			//prevents vertex out of range
			if (i != 0) {
				//adds the first triangle of the horizontal rectangles which make up the sphere
				AddTri(m_vertices[point1], m_vertices[point2], m_vertices[point1 + 1]);
			}

			//prevents vertex out of range
			if (i != (a_nSubdivisions-1)) {
				//adds the corresponding calculated triangle
				AddTri(m_vertices[point1 + 1], m_vertices[point2], m_vertices[point2 + 1]);
			}


			//incrementing points
			point1++;
			point2++;

		}
	}




	

	
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}