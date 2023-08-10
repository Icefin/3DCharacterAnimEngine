#pragma once
#include "Particle.h"

namespace pa
{
	class Spring
	{
	public :
		Spring(float stiffness, float damping, float restLength) :
			_stiffness(stiffness), _damping(damping), _restLength(restLength) { }

		void	setParticles(Particle* left, Particle* right);
		void	setConstants(float stiffness, float damping);
		void	applySpringForce(float deltaTime);

	private :
		Particle* _left = nullptr;
		Particle* _right = nullptr;

		float _stiffness;
		float _damping;
		float _restLength;
	};
}

