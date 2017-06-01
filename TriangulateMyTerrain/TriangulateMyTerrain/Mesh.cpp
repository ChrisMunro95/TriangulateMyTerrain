#include "Mesh.h"


Mesh::Mesh(int sizeX, int sizeY) : dimX_(sizeX), dimY_(sizeY)
{
	genElements();
	genUVcoords();

	for (int i = 0; i < numVerts_; i++){
		normals_.push_back(glm::vec3(0, 1, 0));
	}
}


Mesh::~Mesh()
{
}


void Mesh::setGridSize(int sizeX, int sizeY)
{
	dimX_ = sizeX;
	dimY_ = sizeY;
}

void Mesh::genElements()
{
	bool skip = false;

	//This algorithm will create an Alternating directional Grid

	//every loop it will work out the element index of 2 triangles
	//that make a Quad

	//Step and K are the control variable 
	//Step tells us how far we have to 'Step' to the get the next valid index
	//K is the final 'true' value we push into the vector after some tweaks are made

	for (int j = 0; j < dimY_ - 1; j++){
		for (int i = 0; i < dimX_ - 1; i++){

			int step = j * dimX_ + i;
			int k = 0;

			if (skip == false){

				k = step;
				elements_.push_back(k);

				k = k + 1;
				elements_.push_back(k);

				k = step + dimX_ + 1;
				elements_.push_back(k); // First Tri

				k = k - (dimX_ + 1);
				elements_.push_back(k);

				k = k + dimX_ + 1;
				elements_.push_back(k);

				k = k - 1;
				elements_.push_back(k); // Second Tri

				//Quad done

				skip = true;
			}
			else{

				k = step + dimX_;
				elements_.push_back(k);

				k = k - dimX_;
				elements_.push_back(k);

				k = k + 1;
				elements_.push_back(k); // First Tri

				k = step + dimX_;
				elements_.push_back(k);

				k = k - dimX_ + 1;
				elements_.push_back(k);

				k = k + dimX_;
				elements_.push_back(k); // Second Tri

				//Quad done

				skip = false;

			}

		}

	}
}

void Mesh::genUVcoords()
{
	//control variables
	glm::vec2 uvTemp = glm::vec2(0, 0);
	float U_increment = 0;
	float V_increment = (float)dimY_ / numVerts_;

	//deviding the grid size X or Y by the total number of vertices in the grid
	//will give us perfect 'segment' value with 0 - 1.0 range so this value 
	//can just be added on each iteration through the grid X/Y giving us perfect
	//usable UV coordinates

	for (int y = 0; y < dimY_; y++){
		for (int x = 0; x < dimX_; x++){

			uvTemp.x = U_increment;

			UVcoords_.push_back(uvTemp);

			U_increment += (float)dimX_ / numVerts_;

			//ensure not to go over 1.0
			if (U_increment >= 1.0f){
				U_increment = 1.0f;
			}

		}

		uvTemp.y = V_increment;

		V_increment += (float)dimY_ / numVerts_;

		//ensure not to go over 1.0
		if (V_increment >= 1.0f){
			V_increment = 1.0f;
		}

		U_increment = 0;

	}
}

void Mesh::genPositions(std::vector<glm::vec3> &cp1, std::vector<glm::vec3> &cp2)
{
	glm::vec3 pos = glm::vec3(0, 0, 0);
	unsigned int temp0 = 0;
	unsigned int temp1 = 0;

	for (int i = 0; i < numVerts_; i++){

		if (UVcoords_[i].x < 0.5){

			pos = bezierSurface(cp1, UVcoords_[temp0].x, UVcoords_[temp0].y);
			positions_.push_back(pos);
			temp0 += 2;

		}
		else{
			pos = bezierSurface(cp2, UVcoords_[temp1].x, UVcoords_[temp1].y);
			positions_.push_back(pos);
			temp1 += 2;
		}

	}
}

void Mesh::setPositionsArray(std::vector<glm::vec3> positions)
{
	positions_ = positions;
}


//This function will calculate the vertex normals by taking vertex positions,
//creating vectex edges (the line connection one vertex to another) form the tri,
//and get the cross product of the two edges.
//Should be used after the meshes positions have changed
void Mesh::normalize()
{

	for (auto &n : normals_){
		n = glm::vec3(0, 0, 0);
	}

	for (int i = 0; i < numElements_; i += 3){

		//extract positions
		const glm::vec3 p1 = positions_[elements_[i]];
		const glm::vec3 p2 = positions_[elements_[i + 1]];
		const glm::vec3 p3 = positions_[elements_[i + 2]];

		//extract the edges
		const glm::vec3 e1 = p2 - p1;
		const glm::vec3 e2 = p3 - p1;

		//calc the normal
		glm::vec3 tempNorm = glm::cross(e1, e2);

		//add the normal to all the vertices in this trianlge
		normals_[elements_[i]]	   += tempNorm;
		normals_[elements_[i + 1]] += tempNorm;
		normals_[elements_[i + 2]] += tempNorm;
	}

}



void Mesh::smoothNormals()
{

	for (int i = 0; i < numElements_; i += 3){

		//extract the current normals
		const auto n0 = normals_[elements_[i]];
		const auto n1 = normals_[elements_[i + 1]];
		const auto n2 = normals_[elements_[i + 2]];

		//add and normalize the some of the other normals related to the same triangle
		const auto edge1 = glm::normalize(n0 + n1);
		const auto edge2 = glm::normalize(n0 + n2);

		const auto normal = glm::normalize(edge1 + edge2);

		//add the newly created normal which is a sum of the two edge normals
		normals_[elements_[i]] = normal;
		normals_[elements_[i + 1]] = normal;
		normals_[elements_[i + 2]] = normal;


	}

}

//Apply some randomized value to vertex position, to create a roughness to the mesh,
//The higher the scaleValue the more roughness is appiled i recommend a value from 1.0 - 5.0
//Scale Value needed when not using fractMotion.
//fractmotion may produce a better look - no scale value needed

void Mesh::applyNoise(const float scaleValue, bool fractMotion)
{

	for (size_t i = 0; i < positions_.size() - 1; i++){

		if (fractMotion == true){

			unsigned int octaves = 8;
			float frequency = 1.0f;
			float amplitude = 1.0f;
			float lacunarity = 2.0f;
			float persistence = 0.5f;

			float result = 0.f;

			for (unsigned int j = 0; j < octaves; j++){
				result = noise(positions_[i].x * frequency, positions_[i].z * frequency) * amplitude;
				frequency *= lacunarity;
				amplitude *= persistence;
			}

			result = result * 500.f;

			positions_[i] += result;

		}
		else{

			const float s = noise(positions_[i].x, positions_[i].z);
			const float t = noise(positions_[i + 1].x, positions_[i].z);

			const float u = noise(positions_[i].x, positions_[i + 1].z);
			const float v = noise(positions_[i + 1].x, positions_[i + 1].z);

			const float a1 = interpolateNoise(s, t, positions_[i].x);
			const float a2 = interpolateNoise(u, v, positions_[i].x);

			float finalLerp = interpolateNoise(a1, a2, positions_[i].z);

			finalLerp = finalLerp * scaleValue;

			positions_[i] += finalLerp;

		}
	}


}

//BEZIER FUNCTIONS
glm::vec3 Mesh::cubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float u)
{
	return (1 - u) * (1 - u) * (1 - u) * p0 +
		3 * u  * (1 - u) * (1 - u) * p1 +
		3 * u * u * (1 - u) * p2 +
		u * u * u * p3;
}

std::vector<glm::vec3> Mesh::sampleBezierCurve(std::vector<glm::vec3> cps)
{
	std::vector<glm::vec3> curve;

	for (unsigned int j = 0; j < curve.size(); j++){

		float u = (float)j / (float)(curve.size() - 1); //Bezier Parameter

		curve.push_back(cubicBezier(cps[0], cps[1], cps[2], cps[3], u));

	}

	return curve;

}

glm::vec3 Mesh::bezierSurface(std::vector<glm::vec3> &cps, float u, float v)
{
	std::vector<glm::vec3> curve{ cps.size() };

	for (unsigned int j = 0; j < 4; j++){

		curve[j] = cubicBezier(cps[j * 4], cps[j * 4 + 1], cps[j * 4 + 2], cps[j * 4 + 3], u);

	}

	return cubicBezier(curve[0], curve[1], curve[2], curve[3], v);

}

float Mesh::noise(float x, float y)
{
	int n = (int)(x + y * 57);
	n = (n >> 13) ^ n;

	int nn = (n * (n * 60493 + 19990303) + 1376312589) & 0x7fffffff;

	return 1.0f - ((float)nn / 1073741824.0f);
}

float Mesh::interpolateNoise(float a, float b, float x)
{
	float ft = x * 3.1415927f;

	float f = (1.0f - cos(ft)) * 0.5f;

	return a * (1.0f - f) + b * f;
}