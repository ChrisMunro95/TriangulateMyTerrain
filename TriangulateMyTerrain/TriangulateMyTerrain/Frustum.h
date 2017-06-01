#pragma once
#include "glm\glm.hpp"
#include <vector>

struct Plane
{
	glm::vec3 normal;
	float d;
};

class Frustum
{
public:

	Frustum();
	~Frustum();

	static enum {OUTSIDE, INTERSECT, INSIDE};
	void NormalizePlane(Plane &plane);
	std::vector<Plane> extractPlanes(const glm::mat4 &pv_matrix, bool normalize);
	float distanceToPoint(const Plane &plane, const glm::vec3 point);

	bool checkIntersection(const glm::vec3 &point);


private:

	enum {Left = 0, Right, Top, Bot, NearP, FarP};

	std::vector<Plane> planes_;

};

