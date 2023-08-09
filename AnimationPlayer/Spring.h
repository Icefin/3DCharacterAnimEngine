#pragma once
#include "Particle.h"

namespace pa
{
	class Spring
	{
	public :
		Spring(float stiffness, float damping, float restLength) :
			_stiffness(stiffness), _damping(damping), _restLength(restLength) { }

		Particle* getLeftParticle(void) { return _left; }
		Particle* getRightParticle(void) { return _right; }

		void	setParticles(Particle* left, Particle* right);
		void	setConstants(float stiffness, float damping);
		void	applyForce(float deltaTime);

	private :
		Particle* _left;
		Particle* _right;

		float _stiffness;
		float _damping;
		float _restLength;
	};
}

