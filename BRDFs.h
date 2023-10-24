#pragma once
#include <cassert>
#include "Math.h"
//TERMINOLOGY:
// cd = DIFFUSE COLOR (RGBColor)
// kd = DIFFUSE REFLECTANCE (float)
// exp = phong exponent
// ks = specular reflectance factor

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			auto rho = cd * kd;
			return rho / PI; //week 3 ppt, slide 35
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			auto rho = cd * kd;
			return rho / PI; //week 3 ppt, slide 35
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			//todo: W3
			//const Vector3 reflect = l - (2.f *  (Vector3::Dot(n, l) * n)); //powerpoint week 3, slide 47
			const Vector3 reflect = Vector3::Reflect(l, n); //replaced with built in function of framework
			float cosAlpha = Vector3::Dot(reflect, v);
			if (cosAlpha < 0.f) cosAlpha = 0.f; //cannot be negative
			
			const float value = ks * powf(cosAlpha, exp);//also week 3, slide 47

			return ColorRGB{ value, value, value };

		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			const ColorRGB metalness{ 1.f - f0.r, 1.f - f0.g, 1 - f0.b }; //replacing f0 in the fresnel calculation with this
			const ColorRGB Fresnel{ f0 + (metalness * powf(1 - Vector3::Dot(h,v), 5)) }; //added extra brackets before metalness for visual clarity
			return Fresnel;
		}



		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			//https://ix.cs.uoregon.edu/~hank/441/lectures/pbr_slides.pdf personal source to recheck
			const float roughnessSquared{ Square( roughness )};
			const float roughnessDoubleSquared{ Square(roughnessSquared) }; //not sure if this is necessary
			const float dotnh{ Vector3::Dot(n,h) };

			const float  denominator{ PI * Square(Square(dotnh) * (roughnessDoubleSquared - 1.f) + 1.f )};
			const float trowbridgeReitzGGX{ roughnessDoubleSquared / denominator };
	
			return trowbridgeReitzGGX;
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			const float dotnv{ Vector3::Dot(n,v) };
			const float overshadowing{ dotnv	 / ((dotnv * (1.f - roughness)) + roughness) };
			return overshadowing;//term from slide 70 w3
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			//todo: W3
			//assert(false && "Not Implemented Yet");
			//direct lighting w3 slide 70
			const float roughnessSquared{ Square(roughness) };
			const float k{ Square(roughnessSquared + 1.f) / 8.f };
			return GeometryFunction_SchlickGGX(n, v, k) * GeometryFunction_SchlickGGX(n, l, k);
			//return GeometryFunction_SchlickGGX(n, v, roughness) * GeometryFunction_SchlickGGX(n, l, roughness);
		}

	}
}