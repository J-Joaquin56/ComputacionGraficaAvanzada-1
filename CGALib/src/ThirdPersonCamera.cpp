#include "Headers/ThirdPersonCamera.h"

ThirdPersonCamera::ThirdPersonCamera(){
    pitch = glm::radians(20.0f);
    yaw = 0.0f;
    angleAroundTarget = 0.0f;
    angleTarget = 0.0;
    distanceFromTarget = 1.0f;
    sensitivity = SENSITIVTY;
    worldUp = glm::vec3(0.0, 1.0, 0.0);
    updateCamera();
}

void ThirdPersonCamera::mouseMoveCamera(float xoffset, float yoffset, float dt){
    // Camera controls
    float cameraSpeed = sensitivity * dt;

    // Calculate pitch
    pitch += yoffset * cameraSpeed;
    // Calculate Angle Arround
    angleAroundTarget -= xoffset * cameraSpeed;
    if(pitch > M_PI / 2)
        pitch = M_PI / 2 - 0.01;
    if(pitch < -M_PI / 2)
        pitch = -M_PI / 2 + 0.01;

    // Limita el ángulo de inclinación para no ver debajo del mapa
    float minPitch = glm::radians(10.0f);  // Mínimo ángulo de inclinación
    float maxPitch = glm::radians(60.0f);  // Máximo ángulo de inclinación

    if(pitch > maxPitch)
        pitch = maxPitch;
    if(pitch < minPitch)
        pitch = minPitch;

    // Limita el ángulo alrededor del objetivo
    float maxAngleAroundTarget = glm::radians(120.0f); // Ajusta según lo necesites
    if(angleAroundTarget > maxAngleAroundTarget)
        angleAroundTarget = maxAngleAroundTarget;
    if(angleAroundTarget < -maxAngleAroundTarget)
        angleAroundTarget = -maxAngleAroundTarget;

    updateCamera();
}

void ThirdPersonCamera::scrollMoveCamera(float soffset, float dt){
    // Camera controls
    float cameraSpeed = sensitivity * dt;
    // Calculate zoom
    float zoomLevel = soffset * cameraSpeed;
    distanceFromTarget -= zoomLevel;
    updateCamera();
}

void ThirdPersonCamera::updateCamera(){
    //Calculate Horizontal distance
    float horizontalDistance = distanceFromTarget * cos(pitch);
    //Calculate Vertical distance
    float verticalDistance = distanceFromTarget * sin(pitch);

    //Calculate camera position
    float theta = angleTarget + angleAroundTarget;
    float offsetx = horizontalDistance * sin(theta);
    float offsetz = horizontalDistance * cos(theta);
    position.x = cameraTarget.x - offsetx;
    position.z = cameraTarget.z - offsetz;
    position.y = cameraTarget.y + verticalDistance;

    yaw = angleTarget - (180 + angleAroundTarget);

    if (distanceFromTarget < 0)
    	front = glm::normalize(position - cameraTarget);
    else
    	front = glm::normalize(cameraTarget - position);

    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));

    // Calcula el vector "frente" de la cámara
    if (distanceFromTarget < 0)
        front = glm::normalize(position - cameraTarget);
    else
        front = glm::normalize(cameraTarget - position);

    // Calcula los vectores "derecha" y "arriba"
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));

    // **Desplaza la cámara ligeramente a la derecha**
    float horizontalOffset = 1.0f; // Ajusta este valor según lo necesites
    position += this->right * horizontalOffset;
}
