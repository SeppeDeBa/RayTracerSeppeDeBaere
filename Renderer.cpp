//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

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
	m_ScreenMiddle.z = 1.f; //todo 1: ask if this if fine?
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	//week2: calculate fov and cameraToWorld
	const float fov{ tanf((camera.fovAngle * TO_RADIANS) / 2) };
	const Matrix cameraToWorld{ camera.CalculateCameraToWorld() };



	for (int px{}; px < m_Width; ++px)
	{
		const float rayDirectionX{ (((2 * (px + 0.5f)) / m_Width - 1) * m_AspectRatio) * fov };
		for (int py{}; py < m_Height; ++py)
		{
			const float rayDirectionY{ (1.f - (2 * (py + 0.5f) / m_Height)) * fov };
			//old gradient code used to add to finalColor
			//float gradient = px / static_cast<float>(m_Width);
			//gradient += py / static_cast<float>(m_Width);
			//gradient /= 2.0f;

			Vector3 finalRayVector{ cameraToWorld.TransformVector(rayDirectionX, rayDirectionY,1).Normalized() };
				

			//old code, keeping for backup
			//Vector3 finalRayVector{ rayDirectionX
			//					,	rayDirectionY
			//					,	1.f }; //constant 1; could be 0 but making 1 to add blue to match the renderer example
			//finalRayVector.Normalize();
		

			Ray viewRay{ camera.origin, finalRayVector };


			//show ray direction through colours:
			//ColorRGB finalColor{ finalRayVector.x
			//					,finalRayVector.y
			//					,finalRayVector.z };

			//Update Color in Buffer

			ColorRGB finalColor{};
			//finalColor.MaxToOne();

			//hitRecord containing closest hit
			HitRecord closestHit{};
			
			pScene->GetClosestHit(viewRay, closestHit);
			if (closestHit.didHit) //if not, color will stay black
			{
				finalColor = materials[closestHit.materialIndex]->Shade();


			//Light
			for (const Light& light : lights)
			{
				const Vector3 offset = closestHit.normal * 0.001f;
				Vector3 LightDirection = LightUtils::GetDirectionToLight(light, closestHit.origin);
				const float lightRayNormalized{ LightDirection.Normalize() };
				const Ray lightRay{ closestHit.origin + offset, LightDirection, 0.0001f, lightRayNormalized }; //0.0001f is default but filling it in anyway
				if (pScene->DoesHit(lightRay))
				{
					finalColor *= 0.5f;
				}
			}

			//add final colors
			finalColor.MaxToOne();
			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));




			}


	



		}
	}


	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
