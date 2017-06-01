#pragma once

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

class Mesh
{
public:
	Mesh() = default;
	Mesh(int sizeX, int sizeY);
	~Mesh();

	void setGridSize(int sizeX, int sizeY);

	int getSizeX() const { return dimX_; };
	int getSizeY() const { return dimY_; };

	void genPositions(std::vector<glm::vec3> &cp1, std::vector<glm::vec3> &cp2);
	void normalize();
	void smoothNormals();
	void applyNoise(const float scaleValue, bool fractMotion);

	std::vector<unsigned int> getElementsArray() const { return elements_; };
	std::vector<glm::vec2> getUVcoordsArray() const { return UVcoords_; };
	std::vector<glm::vec3> getPositionsArray() const { return positions_; };
	std::vector<glm::vec3> getNormalsArray() const { return normals_; };
 
	int getElementCount() const { return elements_.size(); };
	int getVertexCount() const { return numVerts_; };
	int getTriCount() const { return numTri_; };

	void setPositionsArray(std::vector<glm::vec3> positions);


private:

	int dimX_ = 0;
	int dimY_ = 0;

	const int numVerts_ = dimX_ * dimY_;
	const int numQuads_ = (dimX_ - 1) * (dimY_ - 1);
	const int numTri_ = 2 * numQuads_;
	const int numElements_ = numTri_ * 3;

private:
	void genElements(); 
	void genUVcoords();
	
	float noise(float x, float y);
	float interpolateNoise(float a, float b, float x);

	//Bezier functions
	glm::vec3 Mesh::cubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, 
								glm::vec3 p3, float u);
	std::vector<glm::vec3> Mesh::sampleBezierCurve(std::vector<glm::vec3> cps);
	glm::vec3 Mesh::bezierSurface(std::vector<glm::vec3> &cps, float u, float v);

private:
	std::vector<unsigned int> elements_;
	std::vector<glm::vec2> UVcoords_;
	std::vector<glm::vec3> positions_;
	std::vector<glm::vec3> normals_;

	std::vector<std::vector<glm::vec3>> control_points_;
};

