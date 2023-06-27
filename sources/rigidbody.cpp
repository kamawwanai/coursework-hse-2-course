#include "rigidbody.hpp"

RigidBody::RigidBody() {
    output.open("rb.csv");
    output << "t, x, y, z, psi, theta, gamma, alpha" << std::endl;
    transformGlobalToBody = glm::mat3(1.0F);
    transformWindToBody = glm::mat3(1.0F);
}

RigidBody::RigidBody(const glm::vec3& initialPosition) {
    RigidBody();  // NOLINT
    pos = initialPosition;
}

auto RigidBody::transormCoordinates(CoordinateSystem oldCS,
                                    CoordinateSystem newCS, glm::vec3 vec)
    -> glm::vec3 {
    glm::mat3 transform;
    if (oldCS == CoordinateSystem::Wind && newCS == CoordinateSystem::Global) {
        transform = glm::transpose(transformGlobalToBody) * transformWindToBody;
    } else if (oldCS == CoordinateSystem::Global &&
               newCS == CoordinateSystem::Wind) {
        transform = glm::transpose(transformWindToBody) * transformGlobalToBody;
    } else if (oldCS == CoordinateSystem::Body &&
               newCS == CoordinateSystem::Global) {
        transform = glm::transpose(transformGlobalToBody);
    }
    return transform * vec;
}

void RigidBody::update(const glm::vec3& force,  // NOLINT
                       const glm::vec3& moment) {
    vel += force / mass * dt;
    pos += vel * dt;
    omega[0] +=
        static_cast<float>((moment[0] - (Iz - Iy) * omega[1] * omega[2]) / Ix);
    omega[1] += (moment[1] - (Ix - Iz) * omega[0] * omega[2]) / Iy;
    omega[2] += (moment[2] - (Iy - Ix) * omega[0] * omega[1]) / Iz;
    gamma += (omega[0] -
              tan(psi) * (omega[1] * cos(gamma) - omega[2] * sin(gamma))) *
             dt;
    theta += (omega[0] * cos(gamma) + omega[1] * sin(gamma)) * dt;
    psi +=
        1 / cos(theta) * (omega[1] * cos(gamma) - omega[2] * sin(gamma)) * dt;

    transformGlobalToBody = {
        {cos(theta) * cos(psi), sin(theta), -cos(theta) * sin(psi)},
        {-cos(gamma) * sin(theta) * cos(psi) + sin(gamma) * sin(psi),
         cos(gamma) * cos(theta),
         cos(gamma) * sin(theta) * sin(psi) + sin(gamma) * cos(psi)},
        {-sin(gamma) * sin(theta) * cos(psi) + cos(gamma) * sin(psi),
         -sin(gamma) * cos(theta),
         -sin(gamma) * sin(theta) * sin(psi) + cos(gamma) * cos(psi)}};
    PSI = acos(vel[0] / sqrt(vel[0] * vel[0] + vel[2] * vel[2]));
    if (vel[2] > 0) {
        PSI *= -1;
    }
    THETA = acos((vel[0] * vel[0] + vel[2] * vel[2]) /
                 (sqrt(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]) *
                  sqrt(vel[0] * vel[0] + vel[2] * vel[2])));
    if (vel[1] < 0) {
        THETA *= -1;
    }

    // glm::vec3 bodyXGlobCS =
    //     glm::transpose(transformGlobalToBody) * glm::vec3{1, 0, 0};
    // glm::vec3 bodyYGlobCS =
    //     glm::transpose(transformGlobalToBody) * glm::vec3{0, 1, 0};
    // glm::vec3 bodyZGlobCS =
    //     glm::transpose(transformGlobalToBody) * glm::vec3{0, 0, 1};

    glm::vec3 velBodyCS = transformGlobalToBody * vel;
    glm::vec3 projVelXYBodyCS = glm::vec3{velBodyCS[0], velBodyCS[1], 0.0F};
    alpha = acos(glm::dot(projVelXYBodyCS, glm::vec3{1, 0, 0}) /
                 glm::length(projVelXYBodyCS));
    if (projVelXYBodyCS[1] > 0) {
        alpha *= -1;
    }
    beta = acos(glm::dot(velBodyCS, projVelXYBodyCS) /
                (glm::length(velBodyCS) * glm::length(projVelXYBodyCS)));
    if (velBodyCS[2] < 0) {
        beta *= -1;
    }

    transformWindToBody = {
        {cos(alpha) * cos(beta), sin(alpha), -cos(alpha) * sin(beta)},
        {-sin(alpha) * cos(beta), cos(alpha), sin(alpha) * sin(beta)},
        {sin(beta), 0.0F, cos(beta)}};
}

void RigidBody::logging(const double& t) {
    output << t << ", " << pos[0] << ", " << pos[1] << ", " << pos[2] << ", "
           << psi << ", " << theta << ", " << gamma << ", " << alpha
           << std::endl;
}

RigidBody::~RigidBody() { output.close(); }