#pragma once
#include "CommonTypes.h"
#include "Particle.h"

namespace pa
{
	class Softbody
	{
		enum class SpringType :uint8
		{
			Structural = 0,
			Shear,
			Flexion
		};

		struct Spring
		{
			SpringType	type;
			float		restLength;
			Particle* left;
			Particle* right;
		};

		class SoftBody
		{
		};
	};
}