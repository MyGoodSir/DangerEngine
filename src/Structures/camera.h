#pragma once
#include <Danger.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <util/Logger.h>

#include <string>
#include <vector>


//camera data
struct Camera{
    glm::vec3 pos;
    glm::vec3 front;//forward direction
    glm::vec3 up; //positive y axis in camera space as worldspace direction
    glm::vec3 right;//positive x axis in camera space as worldspace direction
    glm::vec3 world_up;//direction of positive y axis of world
    float yaw; //rotation around cameraspace y axis
    float pitch; //rotation around cameraspace x axis
    //roll will be rotation around cameraspace z axis 
    

    float speed; //movement speed
    float mouse_sens; //look speed
    float zoom; //fov
    glm::mat4 projection, world; //transformation matricies 
};

//default values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENS = 0.1f;
const float ZOOM = 45.0f;

//movement direction enum
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
    //initialize static global values
    static void Init(){}
    
    //change front, right, and up vectors based on updated yaw and pitch values
    //only updates rotation
    static void UpdateVecs(Camera &cam)
    {
        cam.front.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
        cam.front.y = sin(glm::radians(cam.pitch));
        cam.front.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
        cam.front = glm::normalize(cam.front);
        cam.right = glm::normalize(glm::cross(cam.front, cam.world_up));
        cam.up = glm::normalize(glm::cross(cam.right, cam.front));
    }

    //creates a new camera object
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

    //creates camera view transformation matrix
    static glm::mat4 GetViewMatrix(Camera cam)
    {
        return glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);
    }

    //moves camera
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

    //update camera position based on mouse offset
    static void UpdateEulerAngles(Camera &cam, float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= cam.mouse_sens;
        yoffset *= cam.mouse_sens;

        cam.yaw += xoffset;
        cam.pitch += yoffset;

        // cant look up more than 90 degrees or down more than 90 degrees. prevents the camera from flipping upside-down
        if (constrainPitch)
        {
            if (cam.pitch > 89.0f)
                cam.pitch = 89.0f;
            if (cam.pitch < -89.0f)
                cam.pitch = -89.0f;
        }

        
        UpdateVecs(cam);
    }

    //changes fov
    static void UpdateZoom(Camera &cam, float yoffset)
    {
        cam.zoom -= (float)yoffset;
        if (cam.zoom < 1.0f)
            cam.zoom = 1.0f;
        if (cam.zoom > 45.0f)
            cam.zoom = 45.0f;
    }


    //oblique view frustum near plane clipping
    //used to prevent camera occlusion when less than the full view frustum needs to be displayed
    //diagram: 
    //(p = portal, o = obstruction, c = camera, | = nearplane, .:* = visible area)

    /*[][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]
    []                      ....      []                          ....      []
    []                ....:::::       []                      |.::::::      []
    []           ..:::::p::**         []                      |p::::::      []
    []       |::::::::**p             []                o     |p::::::      []
    []     C |:::::o    p             []       C        o     |p::::::      []
    []       |:::::::.. p             []                      |p::::::      []
    []           **:::::::....        []                      |:::::::      []
    [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][]*/


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