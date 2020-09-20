#pragma once
#include <Danger.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <util/Logger.h>

#include <string>
#include <vector>


struct Camera{
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    float yaw;
    float pitch;
    // camera options
    float speed;
    float mouse_sens;
    float zoom;
    glm::mat4 projection, world;
    float fov;
};
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENS = 0.1f;
const float ZOOM = 45.0f;

enum Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class CameraManager{
    public:
    static void Init(){}
    //make sure this properly transforms the object
    static void UpdateVecs(Camera &cam)
    {
        cam.front.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
        cam.front.y = sin(glm::radians(cam.pitch));
        cam.front.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
        cam.front = glm::normalize(cam.front);
        cam.right = glm::normalize(glm::cross(cam.front, cam.world_up));
        cam.up = glm::normalize(glm::cross(cam.right, cam.front));
    }

    static Camera Create(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
    {
        Camera cam = {
            position,
            glm::vec3(0.0f, 0.0f, -1.0f),
            up,
            glm::vec3(-1.0f, 0.0f, 0.0f),
            up,
            yaw,
            pitch,
            SPEED,
            SENS,
            ZOOM
        };
        UpdateVecs(cam);
        return cam;
    }

    static glm::mat4 GetViewMatrix(Camera cam)
    {
        return glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);
    }

    static void UpdatePosition(Camera &cam, Movement dir, float dtime){
        float dpos = cam.speed * dtime;
        if (dir == FORWARD)
            cam.pos += cam.front * dpos;
        if (dir == BACKWARD)
            cam.pos -= cam.front * dpos;
        if (dir == LEFT)
            cam.pos -= cam.right * dpos;
        if (dir == RIGHT)
            cam.pos += cam.right * dpos;
        if (dir == UP)
            cam.pos += cam.up * dpos;
        if (dir == DOWN)
            cam.pos -= cam.up * dpos;
    }

    static void UpdateEulerAngles(Camera &cam, float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= cam.mouse_sens;
        yoffset *= cam.mouse_sens;

        cam.yaw += xoffset;
        cam.pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (cam.pitch > 89.0f)
                cam.pitch = 89.0f;
            if (cam.pitch < -89.0f)
                cam.pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateVecs(cam);
    }

    static void UpdateZoom(Camera &cam, float yoffset)
    {
        cam.zoom -= (float)yoffset;
        if (cam.zoom < 1.0f)
            cam.zoom = 1.0f;
        if (cam.zoom > 45.0f)
            cam.zoom = 45.0f;
    }

    static void ClipNearPlaneOblique(Camera &cam, glm::vec3 position, glm::vec3 normal) {
        glm::vec4 clip_position = (cam.world * glm::vec4(position, 1));//adjust position from cameraspace to worldspace
        glm::vec4 clip_normal = (cam.world * glm::vec4(normal, 1));//adjust normal from cameraspace to worldspace
        
        glm::vec4 plane = { clip_normal.x, 
                            clip_normal.y, 
                            clip_normal.z, 
                            -1.0 * glm::dot(glm::vec3(clip_position.x, clip_position.y, clip_position.z), glm::vec3(clip_normal.x, clip_normal.y, clip_normal.z)) };

        glm::vec4 q = glm::inverse(cam.projection) * glm::vec4((plane.x < 0.0f ? 1.0f : -1.0f), (plane.x < 0.0f ? 1.0f : -1.0f), 1.0f, 1.0f);
        glm::vec4 change = plane * (2.0f / glm::dot(plane, q));

        cam.projection[8] = change.x - cam.projection[12];
        cam.projection[9] = change.y - cam.projection[13];
        cam.projection[10] = change.z - cam.projection[14];
        cam.projection[11] = change.w - cam.projection[15];
    }



    
};