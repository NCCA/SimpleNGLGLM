#ifndef SCENEDATA_H_
#define SCENEDATA_H_
#include <ngl/Types.h>
#include <glm/glm.hpp>

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    nglglm::Real roughness;
};

struct Light
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};



#endif
