//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include <execution>
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"
#include "Renderer.h"


#define PARALLEL_EXECUTION
using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
	
	m_AspectRatio = (float)m_Width / m_Height;
	m_ScreenMiddle.x = (float)m_Width / 2.f - 0.5f; //0.5 for pixel offset
	m_ScreenMiddle.y = (float)m_Height / 2.f - 0.5f;//same as .x
	m_ScreenMiddle.z = 1.f; 
}

//old renderer function
//void Renderer::Render(Scene* pScene) const
//{
//	Camera& camera = pScene->GetCamera();
//	auto& materials = pScene->GetMaterials();
//	auto& lights = pScene->GetLights();
//
//	//week2: calculate fov and cameraToWorld
//	const float fov{ tanf((camera.fovAngle * TO_RADIANS) / 2) };
//	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
//
//
//
//	for (int px{}; px < m_Width; ++px)
//	{
//		const float rayDirectionX{ (((2 * (px + 0.5f)) / m_Width - 1) * m_AspectRatio) * fov };
//		for (int py{}; py < m_Height; ++py)
//		{
//			const float rayDirectionY{ (1.f - (2 * (py + 0.5f) / m_Height)) * fov };
//			//old gradient code used to add to finalColor
//			//float gradient = px / static_cast<float>(m_Width);
//			//gradient += py / static_cast<float>(m_Width);
//			//gradient /= 2.0f;
//
//			Vector3 finalRayVector{ cameraToWorld.TransformVector(rayDirectionX, rayDirectionY,1).Normalized() };
//
//
//			//old code, keeping for backup
//			//Vector3 finalRayVector{ rayDirectionX
//			//					,	rayDirectionY
//			//					,	1.f }; //constant 1; could be 0 but making 1 to add blue to match the renderer example
//			//finalRayVector.Normalize();
//
//
//			Ray viewRay{ camera.origin, finalRayVector };
//			ColorRGB finalColor{};
//			HitRecord closestHit{};
//			float reflectionValue{ 1 };
//
//			//show ray direction through colours:
//			//ColorRGB finalColor{ finalRayVector.x
//			//					,finalRayVector.y
//			//					,finalRayVector.z };
//
//			//Update Color in Buffer
//
//			//finalColor.MaxToOne();
//
//			//hitRecord containing closest hit
//
//			//using float 0.001f = DELTA_EPSILON;
//			pScene->GetClosestHit(viewRay, closestHit);
//			if (closestHit.didHit) //if not, color will stay black
//			{
//
//				for (const Light& light : lights)
//				{
//					//check cosine law by doing a dot with the camera normal and the lightDirection.
//					const float LambertCosineLaw{ Vector3::Dot(closestHit.normal, LightUtils::GetDirectionToLight(light, closestHit.origin).Normalized()) };
//
//					if (LambertCosineLaw < 0) //this means that the angle of view is negative, so we can skip this for shading calculations. week 3 ppt slide 40
//					{
//						continue;
//					}
//
//
//					const Vector3 offset = closestHit.normal * 0.001f;
//					Vector3 LightDirection = LightUtils::GetDirectionToLight(light, closestHit.origin);
//					const float lightRayNormalized{ LightDirection.Normalize() };
//					const Ray lightRay{ closestHit.origin + offset, LightDirection, 0.001f , lightRayNormalized }; //0.0001f is default but filling it in anyway
//
//					//no need to continue if lightRay hits.
//					if (pScene->DoesHit(lightRay) && m_ShadowsEnabled) //week 3 ppt slide 40, also moved shadowsEnabled check here
//					{
//						continue;
//					}
//
//					const ColorRGB irradiance{ LightUtils::GetRadiance(light, closestHit.origin) };
//					const ColorRGB BRDF{ materials[closestHit.materialIndex]->Shade(closestHit, LightDirection, -finalRayVector) };
//
//
//
//					switch (m_CurrentLightingMode)
//					{
//					case dae::Renderer::LightingMode::ObservedArea:
//						finalColor += ColorRGB{ LambertCosineLaw, LambertCosineLaw, LambertCosineLaw };
//						break;
//					case dae::Renderer::LightingMode::Radiance:
//						finalColor += irradiance;
//						break;
//					case dae::Renderer::LightingMode::BRDF:
//						finalColor += BRDF;
//						break;
//					case dae::Renderer::LightingMode::Combined:
//						finalColor += (LambertCosineLaw * irradiance * BRDF);
//						break;
//					default:
//						break;
//					}
//
//
//				}
//
//			}
//			else
//			{
//				finalColor.r = 0.f;
//				finalColor.g = 0.f;
//				finalColor.b = 0.f;
//			}
//
//			//add final colors
//			finalColor.MaxToOne();
//			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
//				static_cast<uint8_t>(finalColor.r * 255),
//				static_cast<uint8_t>(finalColor.g * 255),
//				static_cast<uint8_t>(finalColor.b * 255));
//		}
//	}
//
//
//	//@END
//	//Update SDL Surface
//	SDL_UpdateWindowSurface(m_pWindow);
//}


void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };

	const float aspectRatio = m_Width / static_cast<float>(m_Height);

	const float fovAngle = camera.fovAngle * TO_RADIANS;
	const float fov = tan(fovAngle / 2.f);

#if defined (PARALLEL_EXECUTION)
	//parallel logic
	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };
	std::vector<uint32_t> pixelIndices{};

	pixelIndices.reserve(amountOfPixels);
	for (uint32_t index{}; index < amountOfPixels; ++index) pixelIndices.emplace_back(index);

	std::for_each(std::execution::par, pixelIndices.begin(), pixelIndices.end(), [&](int i) {RenderPixel(pScene, i, fov, aspectRatio, cameraToWorld, camera.origin); });

#else
	//Synchronous logic (no threading)

	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };
	for (uint32_t pixelIndex{}; pixelIndex < amountOfPixels; pixelIndex++)
	{
		RenderPixel(pScene, pixelIndex, fov, aspectRatio, cameraToWorld, camera.origin);
	}

#endif
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix cameraToWorld, const Vector3 cameraOrigin) const
{
	auto materials{ pScene->GetMaterials() };

	const uint32_t px{ pixelIndex % m_Width }, py{ pixelIndex / m_Width };

	float rx{ px + 0.5f }, ry{ py + 0.5f };
	float cx{ (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov };
	float cy{ (1 - (2 * (ry / float(m_Height)))) * fov };


	//old gradient code used to add to finalColor
	//float gradient = px / static_cast<float>(m_Width);
	//gradient += py / static_cast<float>(m_Width);
	//gradient /= 2.0f;

	Vector3 finalRayVector{ cameraToWorld.TransformVector(cx, cy,1).Normalized() };


	//old code, keeping for backup
	//Vector3 finalRayVector{ rayDirectionX
	//					,	rayDirectionY
	//					,	1.f }; //constant 1; could be 0 but making 1 to add blue to match the renderer example
	//finalRayVector.Normalize();


	Ray viewRay{ cameraOrigin, finalRayVector };
	ColorRGB finalColor{};
	HitRecord closestHit{};
	float reflectionValue{ 1 };

	//show ray direction through colours:
	//ColorRGB finalColor{ finalRayVector.x
	//					,finalRayVector.y
	//					,finalRayVector.z };

	//Update Color in Buffer

	//finalColor.MaxToOne();

	//hitRecord containing closest hit

	//using float 0.001f = DELTA_EPSILON;
	pScene->GetClosestHit(viewRay, closestHit);
	if (closestHit.didHit) //if not, color will stay black
	{

		for (const Light& light : pScene->GetLights())
		{
			//check cosine law by doing a dot with the camera normal and the lightDirection.
			const float LambertCosineLaw{ Vector3::Dot(closestHit.normal, LightUtils::GetDirectionToLight(light, closestHit.origin).Normalized()) };

			if (LambertCosineLaw < 0) //this means that the angle of view is negative, so we can skip this for shading calculations. week 3 ppt slide 40
			{
				continue;
			}


			const Vector3 offset = closestHit.normal * 0.001f;
			Vector3 LightDirection = LightUtils::GetDirectionToLight(light, closestHit.origin);
			const float lightRayNormalized{ LightDirection.Normalize() };
			const Ray lightRay{ closestHit.origin + offset, LightDirection, 0.001f , lightRayNormalized }; //0.0001f is default but filling it in anyway

			//no need to continue if lightRay hits.
			if (pScene->DoesHit(lightRay) && m_ShadowsEnabled) //week 3 ppt slide 40, also moved shadowsEnabled check here
			{
				continue;
			}

			const ColorRGB irradiance{ LightUtils::GetRadiance(light, closestHit.origin) };
			const ColorRGB BRDF{ materials[closestHit.materialIndex]->Shade(closestHit, LightDirection, -finalRayVector) };



			switch (m_CurrentLightingMode)
			{
			case dae::Renderer::LightingMode::ObservedArea:
				finalColor += ColorRGB{ LambertCosineLaw, LambertCosineLaw, LambertCosineLaw };
				break;
			case dae::Renderer::LightingMode::Radiance:
				finalColor += irradiance;
				break;
			case dae::Renderer::LightingMode::BRDF:
				finalColor += BRDF;
				break;
			case dae::Renderer::LightingMode::Combined:
				finalColor += (LambertCosineLaw * irradiance * BRDF);
				break;
			default:
				break;
			}


		}

	}
	else
	{
		finalColor.r = 0.f;
		finalColor.g = 0.f;
		finalColor.b = 0.f;
	}

	//add final colors
	finalColor.MaxToOne();
	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));

	//SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::CycleLightningMode()
{

	switch (m_CurrentLightingMode)
	{
	case dae::Renderer::LightingMode::ObservedArea:
		m_CurrentLightingMode = LightingMode::Radiance;
		break;
	case dae::Renderer::LightingMode::Radiance:
		m_CurrentLightingMode = LightingMode::BRDF;
		break;
	case dae::Renderer::LightingMode::BRDF:
		m_CurrentLightingMode = LightingMode::Combined;
		break;
	case dae::Renderer::LightingMode::Combined:
		m_CurrentLightingMode = LightingMode::ObservedArea;
		break;
	default:
		std::cout << "Unknown enum reached in CycleLightningMode in Renderer.cpp" << std::endl;
		break;
	}
}
