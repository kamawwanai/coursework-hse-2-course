#pragma once
#include <fstream>
#include <glm/glm.hpp>

enum class CoordinateSystem { Global, Wind, Body };

class RigidBody {
   public:
    RigidBody();
    explicit RigidBody(const glm::vec3& InitialPosition);
    ~RigidBody();
    void update(const glm::vec3& newForce, const glm::vec3& newMoment);
    auto transormCoordinates(CoordinateSystem oldCS, CoordinateSystem newCS,
                             glm::vec3 vec) -> glm::vec3;
    void logging(const double& t);

   private:
    float mass = 15.5F;
    float Ix = 1;
    float Iy = 1;
    float Iz = (1.0F / 12.0F) * mass * (2.35F * 2.35F + 1.55F * 1.55F);
    glm::vec3 pos = {0.0, 0.0, 0.0};
    glm::vec3 vel = {0.0, 0.0, 0.0};
    float psi = 0.0;
    float theta = 0.0;
    float gamma = 0.0;
    float THETA = 0.0;
    float PSI = 0.0;
    float alpha = 0.0;
    float beta = 0.0;
    glm::vec3 omega = {0.0, 0.0, 0.0};
    glm::mat3 transformGlobalToBody;
    glm::mat3 transformWindToBody;
    float dt = 0.001;
    std::ofstream output;
};