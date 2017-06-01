#include "Frustum.h"
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

Frustum::Frustum()
{
	planes_.resize(6);
}


Frustum::~Frustum()
{
}

void Frustum::NormalizePlane(Plane &plane)
{
	float mag;

	mag = sqrt(plane.normal.x * plane.normal.x + plane.normal.y * plane.normal.y + plane.normal.z * plane.normal.z);

	plane.normal.x = plane.normal.x / mag;
	plane.normal.y = plane.normal.y / mag;
	plane.normal.z = plane.normal.z / mag;
	plane.d = plane.d / mag;

}

std::vector<Plane> Frustum::extractPlanes(const glm::mat4 &pv_matrix, bool normalize)
{

	//Left Clipping plane
	planes_[Left].normal.x = pv_matrix[0].w - pv_matrix[0].x;
	planes_[Left].normal.y = pv_matrix[1].w - pv_matrix[1].x;
	planes_[Left].normal.z = pv_matrix[2].w - pv_matrix[2].x;
	planes_[Left].d = pv_matrix[3].w - pv_matrix[3].x;

	//Right Clipping plane
	planes_[Right].normal.x = pv_matrix[0].w - pv_matrix[0].x;
	planes_[Right].normal.y = pv_matrix[1].w - pv_matrix[1].x;
	planes_[Right].normal.z = pv_matrix[2].w - pv_matrix[2].x;
	planes_[Right].d = pv_matrix[3].w - pv_matrix[3].x;

	//Top Clipping plane
	planes_[Top].normal.x = pv_matrix[0].w - pv_matrix[0].y;
	planes_[Top].normal.y = pv_matrix[1].w - pv_matrix[1].y;
	planes_[Top].normal.z = pv_matrix[2].w - pv_matrix[2].y;
	planes_[Top].d = pv_matrix[3].w - pv_matrix[3].y;

	//Bot Clipping plane
	planes_[Bot].normal.x = pv_matrix[0].w - pv_matrix[0].y;
	planes_[Bot].normal.y = pv_matrix[1].w - pv_matrix[1].y;
	planes_[Bot].normal.z = pv_matrix[2].w - pv_matrix[2].y;
	planes_[Bot].d = pv_matrix[3].w - pv_matrix[3].y;

	//Near Clipping plane
	planes_[NearP].normal.x = pv_matrix[0].w - pv_matrix[0].z;
	planes_[NearP].normal.y = pv_matrix[1].w - pv_matrix[1].z;
	planes_[NearP].normal.z = pv_matrix[2].w - pv_matrix[2].z;
	planes_[NearP].d = pv_matrix[3].w - pv_matrix[3].z;

	//Far Clipping plane
	planes_[FarP].normal.x = pv_matrix[0].w - pv_matrix[0].z;
	planes_[FarP].normal.y = pv_matrix[1].w - pv_matrix[1].z;
	planes_[FarP].normal.z = pv_matrix[2].w - pv_matrix[2].z;
	planes_[FarP].d = pv_matrix[3].w - pv_matrix[3].z;

	if (normalize == true){

		for (size_t i = 0; i < 6; i++){

			NormalizePlane(planes_[i]);

		}


	}

	return planes_;

}

bool Frustum::checkIntersection(const glm::vec3 &point)
{
	for (unsigned int i = 0; i < 6; i++){
		if (glm::dot(point, planes_[i].normal + planes_[i].d) <= 0){
			return false;
		}
	}

	return true;
}

float Frustum::distanceToPoint(const Plane &plane, const glm::vec3 point)
{
	return plane.normal.x * point.x + plane.normal.y * point.y + plane.normal.z * point.z + plane.d;
}
