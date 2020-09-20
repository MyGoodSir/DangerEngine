#include "structures.h"
std::vector<std::string> MeshManager::texture_types = std::vector<std::string>();
void InitManagers()
{
    CameraManager::Init();
    MeshManager::Init();
    ModelManager::Init();
    ShaderManager::Init();
}