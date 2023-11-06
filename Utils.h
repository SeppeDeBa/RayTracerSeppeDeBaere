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

				const float distanceRaypointToIntersect{ std::sqrt(sphere.radius * sphere.radius - distanceToRaySquared) };
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
			bool useMullerTrumbore{ false };
			if (useMullerTrumbore)
			{
				const float dotRayNormal{ Vector3::Dot(triangle.normal, ray.direction) };
				if (triangle.cullMode == TriangleCullMode::FrontFaceCulling)
				{
					if (dotRayNormal < 0.f && !ignoreHitRecord)
					{
						return false;
					}
					if (ignoreHitRecord && dotRayNormal > 0.f)
					{
						return false;
					}
				}
				else if (triangle.cullMode == TriangleCullMode::BackFaceCulling)
				{
					if (dotRayNormal > 0.f && !ignoreHitRecord)
					{
						return false;
					}
					if (ignoreHitRecord && dotRayNormal < 0.f)
					{
						return false;
					}
				}
				//todo W5
			//assert(false && "No Implemented Yet!");

			//slide 4
				const Vector3 edgeOne = triangle.v1 - triangle.v0;
				const Vector3 edgeTwo = triangle.v2 - triangle.v0;
				Vector3 h = Vector3::Cross(ray.direction, edgeTwo);

				float a = Vector3::Dot(h, edgeOne);

				const float DELTA = 0.0001f;
				if (a > -DELTA && a < DELTA) return false;


				const float f = 1.f / a;
				const Vector3 s = ray.origin - triangle.v0;
				const float u = f * Vector3::Dot(h, s);
				if (u < 0.f || u > 1.f) return false;

				const Vector3 q = Vector3::Cross(s, edgeTwo);
				const float v = f * Vector3::Dot(q, ray.direction);
				if (v < 0.f || v > 1.f) return false;

				const float t = f * Vector3::Dot( edgeTwo, q);

				if (t > DELTA)hitRecord.t = std::min(hitRecord.t, t);

				hitRecord.didHit = true;
				hitRecord.normal = triangle.normal;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.origin = ray.origin;

			}
			//=====  OLD =====
			else
			{
				//todo W5
				//assert(false && "No Implemented Yet!");

				//slide 4
				const Vector3 a = triangle.v1 - triangle.v0;
				const Vector3 b = triangle.v2 - triangle.v0;
				const float dotRayNormal{ Vector3::Dot(triangle.normal, ray.direction) };
				if (dotRayNormal == 0.f)
				{
					return false;
				}

				//cullModes slide 33
				if (triangle.cullMode == TriangleCullMode::FrontFaceCulling)
				{
					if (dotRayNormal < 0.f && !ignoreHitRecord)
					{
						return false;
					}
					if (ignoreHitRecord && dotRayNormal > 0.f)
					{
						return false;
					}
				}
				else if (triangle.cullMode == TriangleCullMode::BackFaceCulling)
				{
					if (dotRayNormal > 0.f && !ignoreHitRecord)
					{
						return false;
					}
					if (ignoreHitRecord && dotRayNormal < 0.f)
					{
						return false;
					}
				}

				const Vector3 center = (triangle.v0 + triangle.v1 + triangle.v2) / 3.f;

				const Vector3 l = center - ray.origin;


				//slide 5
				const float t = Vector3::Dot(l, triangle.normal) / Vector3::Dot(ray.direction, triangle.normal);
				if (t < ray.min || t > ray.max)
				{
					return false;
				}

				//slide 18
				const Vector3 p = ray.origin + t * ray.direction;

				//check all edges
				const Vector3 edgeA = triangle.v1 - triangle.v0;
				const Vector3 pointToSideV0 = p - triangle.v0;
				if (Vector3::Dot(triangle.normal, Vector3::Cross(edgeA, pointToSideV0)) < 0)
				{
					return false;
				}

				const Vector3 edgeB = triangle.v2 - triangle.v1;
				const Vector3 pointToSideV1 = p - triangle.v1;
				if (Vector3::Dot(triangle.normal, Vector3::Cross(edgeB, pointToSideV1)) < 0)
				{
					return false;
				}

				const Vector3 edgeC = triangle.v0 - triangle.v2;
				const Vector3 pointToSideV2 = p - triangle.v2;
				if (Vector3::Dot(triangle.normal, Vector3::Cross(edgeC, pointToSideV2)) < 0)
				{
					return false;
				}

				hitRecord.didHit = true;
				hitRecord.normal = triangle.normal;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.t = t;
				hitRecord.origin = p;
				return true;

			}

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
			//assert(false && "No Implemented Yet!");

			float distance{ FLT_MAX };
			HitRecord tempHitRecord{};

			for (size_t triangleNr{}; triangleNr < (mesh.indices.size() / 3); ++triangleNr) //3 indices indicate 1 triangle
			{
				const Vector3 v0{ mesh.transformedPositions[mesh.indices[triangleNr * 3]] };
				const Vector3 v1{ mesh.transformedPositions[mesh.indices[triangleNr * 3 + 1]] };
				const Vector3 v2{ mesh.transformedPositions[mesh.indices[triangleNr * 3 + 2]] };

				Triangle triangle{ v0,v1,v2, mesh.transformedNormals[triangleNr] };

				triangle.cullMode = mesh.cullMode;
				triangle.materialIndex = mesh.materialIndex;

				if (HitTest_Triangle(triangle, ray, tempHitRecord, ignoreHitRecord))
				{
					if (ignoreHitRecord)
					{
						return true;
					}
					if (tempHitRecord.didHit && tempHitRecord.t < distance)
					{
						distance = tempHitRecord.t;
						hitRecord = tempHitRecord;
					}
				}
			}
			return hitRecord.didHit;
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
		
			//assert(false && "No Implemented Yet!");
			return light.origin - origin;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target) //does this not return irradiance?
		{
			//todo W3
			//assert(false && "No Implemented Yet!");
			ColorRGB irradiance{};
			switch (light.type)
			{
			case dae::LightType::Point:
				irradiance = light.color * (light.intensity / (light.origin - target).SqrMagnitude());//week 3, slide 25
				break;

			case dae::LightType::Directional:
				irradiance = light.color * light.intensity;//week 3, slide 27w
				break;

			default:
				assert(false && "Default has been hit in GetRadiance in utils.h");
				return {};
				break;
			}
			return irradiance; //could be in one step but i find this slightly more readable for myself
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