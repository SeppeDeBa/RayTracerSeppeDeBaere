#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"
#include <iostream>

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{

			bool useReworkedVersion{ true };
			if (!useReworkedVersion)
			{
				//calculations start
				const Vector3 originToCenter{ sphere.origin - ray.origin };//L (oc)
				const float lengthOriginToPerpSphereCenter{ Vector3::Dot(originToCenter,ray.direction) }; //tca == o to c dot direction
				//discriminant
				const float discriminant = lengthOriginToPerpSphereCenter * lengthOriginToPerpSphereCenter - (Vector3::Dot(originToCenter, originToCenter) - sphere.radius * sphere.radius);
				if (discriminant < 0.f)
				{
					hitRecord.didHit = false;
					//std::cout << "exited at discriminant" << std::endl;
					return false;
				}

				//std::cout << "passed discriminant check" << std::endl;
				const float perpPointToSphereCenter = Vector3::Reject(originToCenter, ray.direction).SqrMagnitude();//od^2
				const float intersectToPerp = sqrt(sphere.radius * sphere.radius - lengthOriginToPerpSphereCenter);//thc
				const float intersectFrontLength = lengthOriginToPerpSphereCenter - intersectToPerp;//t0
				if (intersectFrontLength > ray.max)
				{
					hitRecord.didHit = false;
					//std::cout << "exited at distance Long" << std::endl;
					return false;
				}
				else if (intersectFrontLength < ray.min)
				{
					hitRecord.didHit = false;
					//std::cout << "exited at distance short" << std::endl;
					return false;
				}
				hitRecord.didHit = true;
				//std::cout << "Hit!" << std::endl;
				//add to hitRecord
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.t = intersectFrontLength;
				hitRecord.origin = ray.origin + ray.direction * intersectFrontLength;
				hitRecord.normal = Vector3(sphere.origin, hitRecord.origin).Normalized();
				return true;
			}



			else
			{
				const Vector3 rayOriginToSphereOrigin{ sphere.origin - ray.origin };
				const float hypothenuseSquared{ rayOriginToSphereOrigin.SqrMagnitude() };
				const float side1{ Vector3::Dot(rayOriginToSphereOrigin, ray.direction) };

				const float distanceToRaySquared{ hypothenuseSquared - side1 * side1 };

				//if the distance to the ray is larger than the radius there will be no results
				//    also if equal because that is the exact border of the circle
				if (distanceToRaySquared >= sphere.radius * sphere.radius) {
					hitRecord.didHit = false;
					return false;
				}

				const float distanceRaypointToIntersect{ sqrt(sphere.radius * sphere.radius - distanceToRaySquared) };
				const float distance{ side1 - distanceRaypointToIntersect };

				if (distance < ray.min || distance > ray.max) {
					hitRecord.didHit = false;
					return false;
				}

				hitRecord.didHit = true;
				if (ignoreHitRecord) {
					return true;
				}
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.t = distance;
				hitRecord.origin = ray.origin + distance * ray.direction;
				hitRecord.normal = Vector3(sphere.origin, hitRecord.origin).Normalized();
				return true;
			}


			
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			
			//assert(false && "No Implemented Yet!");
			const float distance{ Vector3::Dot(Vector3{ ray.origin, plane.origin }, plane.normal) / Vector3::Dot(ray.direction, plane.normal) };
			if (distance >= ray.min && distance <= ray.max) {
				hitRecord.didHit = true;
				if (ignoreHitRecord) {
					return true;
				}
				hitRecord.t = distance;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal = plane.normal;
				hitRecord.origin = ray.origin + distance * ray.direction;
				return true;
			}
			hitRecord.didHit = false;
			return false;
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			assert(false && "No Implemented Yet!");
			return {};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			assert(false && "No Implemented Yet!");
			return {};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}