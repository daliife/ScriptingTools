#pragma once
#include "includes.h"
#include <vector>
#include "GraphicsSystem.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

struct TGAInfo //stores info about TGA file
{
    GLuint width;
    GLuint height;
    GLuint bpp; //bits per pixel
    GLubyte* data; //bytes with the pixel information
};

struct TChunk {

    uint32_t magic_id;
    uint32_t num_bytes;
};

struct THeader {

    uint32_t num_vertexs = 0;
    uint32_t num_indices = 0;
    uint32_t primitive_type = 0;
    uint32_t bytes_per_vtx = 0;
    uint32_t bytes_per_idx = 0;
    uint32_t num_subgroups = 0;
    uint32_t dummy2 = 0;
    uint32_t dummy3 = 0;
    char     vertex_type_name[32];
};

static const uint32_t magicHeader =     0x44444444;
static const uint32_t magicVtxs =       0x55554433;
static const uint32_t magicIdxs =       0x55556677;
static const uint32_t magicSubGroups =  0x55556688;
static const uint32_t magicEoF =        0x55558888;

class Parsers {
private:
    static TGAInfo* loadTGA(std::string filename);
public:

    static std::unordered_map<std::string, int> geometries;
    static std::unordered_map<std::string, int> textures;
    static std::unordered_map<std::string, int> materials;
    static std::unordered_map<std::string, int> shaders;

    static GLint parseTexture(std::string filename);

    static bool parseOBJ(std::string filename,
        std::vector<float>& vertices,
        std::vector<float>& uvs,
        std::vector<float>& normals,
        std::vector<unsigned int>& indices);

    static bool parseBin(std::string filename,
        std::vector<float>& vertices,
        std::vector<float>& uvs,
        std::vector<float>& normals,
        std::vector<unsigned int>& indices);

    static int  parseEntity(rapidjson::Value & entity, GraphicsSystem & graphics_system);
    static bool parseJSONLevel(std::string filename, GraphicsSystem& graphics_system);
    static bool parseScene(std::string filename, GraphicsSystem& graphics_system);
};
