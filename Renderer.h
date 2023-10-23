#pragma once

#include <cstdint>
#include "Vector3.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;

		void CycleLightningMode();
		
		void ToggleShadows() { m_ShadowsEnabled = !m_ShadowsEnabled; };

	private:
		//shadows
		enum class LightingMode
		{
			ObservedArea, //lambert cosine law
			Radiance, //incident raidance
			BRDF, //scattering of light
			Combined //Observed Area * Radiance * BRDF
		};

		LightingMode m_CurrentLightingMode{ LightingMode::Combined };

		bool m_ShadowsEnabled{ true };


		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
		float m_AspectRatio{};
		Vector3 m_ScreenMiddle{};
		
	};
}
