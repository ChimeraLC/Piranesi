/* Class that handles all of camera placement for a camera that deals with collisions */

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

/* Camera movement constants */
const float SPEED = 5.0f;
const float SENSITIVITY = 0.05f;
const bool HORIZONTAL_MOVEMENT = true;

class Camera
{
    public:
        /* Camera fields */
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 FrontMove;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 RightMove;
        glm::vec3 WorldUp;
        
        /* Values for rotation*/
        float Yaw;
        float Pitch;

        /* Constructor that creates the camera
         * position - initial position of the camera
         * up - upward direction of the camera
        */
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) :
            Front(glm::vec3(0.0f, 0.0f, -1.0f)),
            Yaw(-90.0f), Pitch(0.0f)
        {
            Position = position;
            WorldUp = up;
            updateCameraVectors();
        }

       /* 
        *  Effects:
        *       Returns the view matrix corresponding to camera location 
        */
        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

       /* 
        *  Effects:
        *       Returns the position of the camera 
        */
        glm::vec3 GetPosition()
        {
            return Position;
        }

       /* 
        *  Effects:
        *       Processes mouse movements recieved by window.
        */
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            xoffset *= SENSITIVITY;
            yoffset *= SENSITIVITY;

            Yaw   += xoffset;
            Pitch += yoffset;

            /* Limit pitch to a given range */
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            /* Update vectors based on movement */
            updateCameraVectors();
        }

        /* Moves the camera based on inputs*/
        void ProcessInputs(glm::vec2 inputs, glm::vec3 *pillarPositions, unsigned int pillarInstances, float pillarWidth)
        {
            float velocity = SPEED;
            Position += FrontMove * inputs.y * velocity;
            Position += RightMove * inputs.x * velocity;
            glm::vec3 pillarPosition;
            float pillarBuffer = pillarWidth / 2 + 1;
            /* Check for collisions TODO: BETTER COLLISION DETECTION */
            for (unsigned int i = 0; i < pillarInstances; i++) {
                pillarPosition = pillarPositions[i];
                if (Position.x > pillarPosition.x - pillarBuffer &&
                    Position.x < pillarPosition.x + pillarBuffer && 
                    Position.z > pillarPosition.z - pillarBuffer &&
                    Position.z < pillarPosition.z + pillarBuffer)
                {
                    /* Finding nearest edge of object */
                    int direction = 0;
                    float closestSide = Position.x - (pillarPosition.x - pillarBuffer);

                    if (Position.z + pillarBuffer - pillarPosition.z < closestSide) {
                        direction = 1;
                        closestSide = Position.z - (pillarPosition.z - pillarBuffer);
                    }

                    if (pillarPosition.x + pillarBuffer - Position.x < closestSide) {
                        direction = 2;
                        closestSide = pillarPosition.x + pillarBuffer - Position.x;
                    }

                    if (pillarPosition.z + pillarBuffer - Position.z < closestSide) {
                        direction = 3;
                        closestSide = pillarPosition.z + pillarBuffer - Position.z; // Unecessary
                    }

                    /* Reposition */
                    switch (direction) {
                        case 0:
                        Position.x = pillarPosition.x - pillarBuffer - 0.1f;
                        break;
                        case 2:
                        Position.x = pillarPosition.x + pillarBuffer + 0.1f;
                        break;
                        case 1:
                        Position.z = pillarPosition.z - pillarBuffer - 0.1f;
                        break;
                        case 3:
                        Position.z = pillarPosition.z + pillarBuffer + 0.1f;
                        break;
                    }
                }
            }
        }
        
    private:
        /* Calculates the front and right vectors based on rotation */
        void updateCameraVectors() {
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            Right = glm::normalize(glm::cross(Front, WorldUp));
            Up = glm::normalize(glm::cross(Right, Front));

            glm::vec3 frontMove;
            frontMove.x = cos(glm::radians(Yaw));
            frontMove.z = sin(glm::radians(Yaw));
            FrontMove = glm::normalize(frontMove);
            RightMove = glm::normalize(glm::cross(FrontMove, WorldUp));
        }
};


#endif