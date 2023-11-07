#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include <iostream>
#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			
			//1. calculate camera axis system
			Vector3 cameraRight{ Vector3::Cross(Vector3::UnitY, forward).Normalized()};
			Vector3 cameraUp{ Vector3::Cross(forward, cameraRight).Normalized() };

			//2. create cameraToWorld Matrix
			Matrix cameraToWorldMatrix
			{
				Vector4{ cameraRight, 0},
				Vector4{ cameraUp, 0 },
				Vector4{ forward, 0 },
				Vector4{ cameraRight, 1 }
			};

			//3. update axis system to calculated system
			right = cameraRight;
			up = cameraUp;



			return cameraToWorldMatrix;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			
			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);


			
			//implement movement
			const float movementSpeed{ 5.f * pTimer->GetElapsed() };
			if(pKeyboardState[SDL_SCANCODE_W])
			{
				this->origin += forward * movementSpeed;
			}
			else if (pKeyboardState[SDL_SCANCODE_S])
			{
				this->origin -= forward * movementSpeed;
			}
			else if (pKeyboardState[SDL_SCANCODE_A])
			{
				this->origin -= right * movementSpeed;
			}
			else if (pKeyboardState[SDL_SCANCODE_D])
			{
				this->origin += right * movementSpeed;
			}


			//implement rotation
			Matrix rotationMatrix{};
			const float rotationSpeed{ 10.f * pTimer->GetElapsed() };
			if ((mouseState & SDL_BUTTON_LMASK) != 0
				&& (mouseState & SDL_BUTTON_RMASK) != 0)
			{
			//	std::cout << "BOTH BUTTONS" << std::endl;
				const Vector3 movement = up * static_cast<float>(-mouseY) * movementSpeed;
				this->origin += movement;
			}
			else if ((mouseState & SDL_BUTTON_RMASK) != 0) //right mouse
			{
				//update members
				totalYaw += mouseX * rotationSpeed;
				totalPitch -= mouseY * rotationSpeed;

				//create rotation matrix
				rotationMatrix = Matrix::CreateRotation(totalPitch, totalYaw, 0);

				//update forward vector, the rest will calculate off of this.
				forward = rotationMatrix.TransformVector(Vector3::UnitZ);
				forward.Normalize();
			}


			else if ((mouseState & SDL_BUTTON_LMASK) != 0)
			{
				totalYaw += mouseX * rotationSpeed;
				const Vector3 movement = forward * static_cast<float>(-mouseY) * movementSpeed;

				rotationMatrix = Matrix::CreateRotation(totalPitch, totalYaw, 0);

				forward = rotationMatrix.TransformVector(Vector3::UnitZ);
				forward.Normalize();

				this->origin += movement;
			}	


		}
	};
}
