#pragma once
#include "Softbody.h"

namespace pa
{
	class Cloth : public Softbody
	{
	public :
		void initialize(int32 gridSize, float distance, const glm::vec3& position);

		void setStructuralSprings(float stiffness, float damping);
		void setShearSprings(float stiffness, float damping);
		void setFlexionSprings(float stiffness, float damping);
		void setParticleMass(float mass);

		void update(float deltaTime) override;
		void render(Shader& shader) override;
		void applyInternalForces() override;
		void applyExternalForces() override;
		void solveConstraints(std::vector<OBB>& constraints) override;

	private :
		std::vector<Particle> _vertices;
		std::vector<Spring> _structuralSprings;
		std::vector<Spring> _shearSprings;
		std::vector<Spring> _flexionSprings;
		float _sideLength;
	};
}

