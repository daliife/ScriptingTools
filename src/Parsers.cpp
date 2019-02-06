#include "Parsers.h"
#include <fstream>
#include "extern.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "components/comp_rotator.h"
#include "components/comp_tag.h"
#include "components/comp_movingplatform.h"
#include <unordered_map>

std::unordered_map<std::string, int> Parsers::geometries;
std::unordered_map<std::string, int> Parsers::textures;
std::unordered_map<std::string, int> Parsers::materials;
std::unordered_map<std::string, int> Parsers::shaders;

void split(std::string to_split, std::string delim, std::vector<std::string>& result) {
    size_t last_offset = 0;
    while (true) {
        //find first delim
        size_t offset = to_split.find_first_of(delim, last_offset);
        result.push_back(to_split.substr(last_offset, offset - last_offset));
        if (offset == std::string::npos) // if at end of string
            break;
        else //otherwise continue
            last_offset = offset + 1;
    }
}

//parses a wavefront object into passed arrays
bool Parsers::parseOBJ(std::string filename, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<float>& normals, std::vector<unsigned int>& indices) {

    std::string line;
    std::ifstream file(filename);
    if (file.is_open())
    {
        //declare containers for temporary and final attributes
        std::vector<lm::vec3> temp_vertices;
        std::vector<lm::vec2> temp_uvs;
        std::vector<lm::vec3> temp_normals;

        //container to store map for indices
        std::unordered_map<std::string, int> indices_map;
        int next_index = 0; //stores next available index

        //parse file line by line
        while (std::getline(file, line))
        {
            //split line string
            std::vector<std::string> words;
            split(line, " ", words);
            for (auto it = words.begin(); it != words.end();)
            {
                if ((*it) == "") it = words.erase(it);
                else it++;
            }

            if (words.empty()) continue; //empty line, skip
            if (words[0][0] == '#')	continue; //first word starts with #, line is a comment

            if (words[0] == "v") { //line contains vertex data
                //read words to floats
                lm::vec3 pos((float)atof(words[1].c_str()),
                    (float)atof(words[2].c_str()),
                    (float)atof(words[3].c_str()));
                //add to temporary vector of positions
                temp_vertices.push_back(pos);
            }
            if (words[0] == "vt") { //line contains texture data
                //read words to floats
                lm::vec2 tex((float)atof(words[1].c_str()),
                    (float)atof(words[2].c_str()));
                //add to temporary vector of texture coords
                temp_uvs.push_back(tex);
            }
            if (words[0] == "vn") { //line contains vertex data
                //read words to floats
                lm::vec3 norm((float)atof(words[1].c_str()),
                    (float)atof(words[2].c_str()),
                    (float)atof(words[3].c_str()));
                //add to temporary vector of normals
                temp_normals.push_back(norm);
            }

            //line contains face-vertex data
            if (words[0] == "f") {
                if (words.size() < 4) continue; // faces with fewer than 3 vertices??!

                bool quad_face = false; //boolean to help us deal with quad faces

                std::vector<std::string> nums; // container used for split indices
                //for each face vertex
                for (int i = 1; i < words.size(); i++) {

                    //check if face vertex has already been indexed
                    if (indices_map.count(words[i]) == 0) {

                        if (words[i] == "") continue;
                        //if not, start by getting all indices
                        nums.clear();
                        split(words[i], "/", nums);
                        int v_ind = atoi(nums[0].c_str()) - 1; //subtract 1 to convert to 0-based array!
                        int t_ind = atoi(nums[1].c_str()) - 1;
                        int n_ind = atoi(nums[2].c_str()) - 1;

                        //add vertices to final arrays of floats
                        for (int j = 0; j < 3; j++)
                            vertices.push_back(temp_vertices[v_ind].value_[j]);
                        for (int j = 0; j < 2; j++)
                            uvs.push_back(temp_uvs[t_ind].value_[j]);
                        for (int j = 0; j < 3; j++)
                            normals.push_back(temp_normals[n_ind].value_[j]);

                        //add an index to final array
                        indices.push_back(next_index);

                        //record that this index is used for this face vertex
                        indices_map[words[i]] = next_index;

                        //increment index
                        next_index++;
                    }
                    else {
                        //face vertex was already added to final arrays
                        //so search for its stored index
                        int the_index = indices_map.at(words[i]); //safe to use as we know it exists
                        //add it to final index array
                        indices.push_back(the_index);
                    }

                    //***CHECK FOR QUAD FACES****
                    //If the face is a quads (i.e. words.size() == 5), we need to create two triangles
                    //out of the quad. We have already created one triangle with words[1], [2] and [3]
                    //now we make another with [4], [1] and [3]. 
                    if (i == 4) {
                        //face-vertex 4 is already added, so we search for indices of 1 and 3 and add them
                        int index_1 = indices_map.at(words[1]);
                        indices.push_back(index_1);
                        int index_3 = indices_map.at(words[3]);
                        indices.push_back(index_3);
                    }

                }

            }

        }
        file.close();
        return true;
    }
    return false;
}

bool Parsers::parseBin(std::string filename, std::vector<float>& vertices, std::vector<float>& uvs, std::vector<float>& normals, std::vector<unsigned int>& indices)
{
    FILE* f = nullptr;
    f = fopen(filename.c_str(), "rb");

    //declare containers for temporary and final attributes
    THeader header;
    std::vector<float> temp_vtxs;

    //parse file line by line
    bool eof_found = false;

    while (!eof_found) {

        //split line string
        TChunk chunk;
        auto bytes_read = fread(&chunk, 1, sizeof(chunk), f);
        assert(bytes_read == sizeof(chunk));

        switch (chunk.magic_id) {

        case magicHeader:

            bytes_read = fread(&header, 1, chunk.num_bytes, f);
            assert(bytes_read == chunk.num_bytes);
            break;

        case magicVtxs:

            temp_vtxs.resize(chunk.num_bytes);
            bytes_read = fread(temp_vtxs.data(), 1, chunk.num_bytes, f);
            assert(bytes_read == chunk.num_bytes);
            break;

        case magicIdxs:

            indices.resize(chunk.num_bytes);
            bytes_read = fread(indices.data(), 1, chunk.num_bytes, f);
            assert(bytes_read == chunk.num_bytes);
            break;

        case magicSubGroups:

            // add subgroups here
            break;

        case magicEoF:

            eof_found = true;
            break;

        default:
            //printf("Unknown chunk data type %08x of %d bytes while reading file %s\n", chunk.magic_id, chunk.num_bytes, filename.c_str());
            break;
        }
    }

    // Replace this with an efficient system without hardcoded sizes!
    int vertex_size = header.bytes_per_vtx / sizeof(float);

    for (unsigned int i = 0; i < temp_vtxs.size() - vertex_size; i = i + vertex_size) {

        vertices.insert(vertices.end(), { temp_vtxs[i], temp_vtxs[i + 1], temp_vtxs[i + 2] });
        normals.insert(normals.end(), { temp_vtxs[i + 3], temp_vtxs[i + 4], temp_vtxs[i + 5] });
        uvs.insert(uvs.end(), { temp_vtxs[i + 6], temp_vtxs[i + 7] });
    }

    fclose(f);
    return true;
}

// load uncompressed RGB targa file into an OpenGL texture
GLint Parsers::parseTexture(std::string filename) {
    std::string str = filename;
    std::string ext = str.substr(str.size() - 4, 4);


    GLuint texture_id;

    if (ext == ".tga" || ext == ".TGA")
    {
        TGAInfo* tgainfo = loadTGA(filename);
        if (tgainfo == NULL) {
            std::cerr << "ERROR: Could not load TGA file" << std::endl;
            return false;
        }

        //generate new openGL texture and bind it (tell openGL we want to do stuff with it)
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id); //we are making a regular 2D texture

                                                  //screen pixels will almost certainly not be same as texture pixels, so we need to
                                                  //set some parameters regarding the filter we use to deal with these cases
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//set the mag filter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //set the min filter
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4); //use anisotropic filtering

                                                                          //this is function that actually loads texture data into OpenGL
        glTexImage2D(GL_TEXTURE_2D, //the target type, a 2D texture
            0, //the base level-of-detail in the mipmap
            (tgainfo->bpp == 24 ? GL_RGB : GL_RGBA), //specified the color channels for opengl
            tgainfo->width, //the width of the texture
            tgainfo->height, //the height of the texture
            0, //border - must always be 0
            (tgainfo->bpp == 24 ? GL_BGR : GL_BGRA), //the format of the incoming data
            GL_UNSIGNED_BYTE, //the type of the incoming data
            tgainfo->data); // a pointer to the incoming data

                            //we want to use mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        //clean up memory
        delete tgainfo->data;
        delete tgainfo;
        return texture_id;
    }
    else {
        std::cerr << "ERROR: No extension or extension not supported" << std::endl;
        return -1;
    }
}

// this reader supports only uncompressed RGB targa files with no colour table
TGAInfo* Parsers::loadTGA(std::string filename)
{
    //the TGA header is 18 bytes long. The first 12 bytes are for specifying the compression
    //and various fields that are very infrequently used, and hence are usually 0.
    //for this limited file parser, we start by reading the first 12 bytes and compare
    //them against the pattern that identifies the file a simple, uncompressed RGB file.
    //more info about the TGA format cane be found at http://www.paulbourke.net/dataformats/tga/

    char TGA_uncompressed[12] = { 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
    char TGA_compare[12];
    char info_header[6];
    GLuint bytes_per_pixel;
    GLuint image_size;

    //open file
    std::ifstream file(filename, std::ios::binary);

    //read first 12 bytes
    file.read(&TGA_compare[0], 12);
    std::streamsize read_header_12 = file.gcount();
    //compare to check that file in uncompressed (or not corrupted)
    int header_compare = memcmp(TGA_uncompressed, TGA_compare, sizeof(TGA_uncompressed));
    if (read_header_12 != sizeof(TGA_compare) || header_compare != 0) {
        std::cerr << "ERROR: TGA file is not in correct format or corrupted: " << filename << std::endl;
        file.close();
        return nullptr;
    }

    //read in next 6 bytes, which contain 'important' bit of header
    file.read(&info_header[0], 6);

    TGAInfo* tgainfo = new TGAInfo;

    tgainfo->width = info_header[1] * 256 + info_header[0]; //width is stored in first two bytes of info_header
    tgainfo->height = info_header[3] * 256 + info_header[2]; //height is stored in next two bytes of info_header

    if (tgainfo->width <= 0 || tgainfo->height <= 0 || (info_header[4] != 24 && info_header[4] != 32)) {
        file.close();
        delete tgainfo;
        std::cerr << "ERROR: TGA file is not 24 or 32 bits, or has no width or height: " << filename << std::endl;
        return NULL;
    }

    //calculate bytes per pixel and then total image size in bytes
    tgainfo->bpp = info_header[4];
    bytes_per_pixel = tgainfo->bpp / 8;
    image_size = tgainfo->width * tgainfo->height * bytes_per_pixel;

    //reserve memory for the image data
    tgainfo->data = (GLubyte*)malloc(image_size);

    //read data into memory
    file.read((char*)tgainfo->data, image_size);
    std::streamsize image_read_size = file.gcount();

    //check it has been read correctly
    if (image_read_size != image_size) {
        if (tgainfo->data != NULL)
            free(tgainfo->data);
        file.close();
        std::cerr << "ERROR: Could not read tga data: " << filename << std::endl;
        delete tgainfo;
        return NULL;
    }

    file.close();

    return tgainfo;
}

bool Parsers::parseJSONLevel(std::string filename,
                             GraphicsSystem& graphics_system) {
    //read json file and stream it into a rapidjson document
    //see http://rapidjson.org/md_doc_stream.html
    std::ifstream json_file(filename);
    rapidjson::IStreamWrapper json_stream(json_file);
    rapidjson::Document json;
    json.ParseStream(json_stream);
    //check if its valid JSON
    if (json.HasParseError()) std::cerr << "JSON format is not valid!" << std::endl;
    //check if its a valid scene file
    if (!json.HasMember("scene")) { std::cerr << "JSON file is incomplete! Needs entry: scene" << std::endl; return false; }
    if (!json.HasMember("directory")) { std::cerr << "JSON file is incomplete! Needs entry: directory" << std::endl; return false; }
    if (!json.HasMember("textures")) { std::cerr << "JSON file is incomplete! Needs entry: textures" << std::endl; return false; }
    if (!json.HasMember("materials")) { std::cerr << "JSON file is incomplete! Needs entry: materials" << std::endl; return false; }
    if (!json.HasMember("lights")) { std::cerr << "JSON file is incomplete! Needs entry: lights" << std::endl; return false; }
    if (!json.HasMember("entities")) { std::cerr << "JSON file is incomplete! Needs entry: entities" << std::endl; return false; }
    if (!json.HasMember("shaders")) { std::cerr << "JSON file is incomplete! Needs entry: shaders" << std::endl; return false; }


    printf("Parsing Scene Name = %s\n", json["scene"].GetString());

    std::string data_dir = json["directory"].GetString();

    //dictionaries
    std::unordered_map<std::string, int> geometries;
    std::unordered_map<std::string, int> textures;
    std::unordered_map<std::string, int> materials;
    std::unordered_map<std::string, std::string> child_parent;

    //geometries
    for (rapidjson::SizeType i = 0; i < json["geometries"].Size(); i++) {
        //get values from json
        std::string name = json["geometries"][i]["name"].GetString();
        std::string file = json["geometries"][i]["file"].GetString();
        //load geometry
        int geom_id = graphics_system.createGeometryFromFile(data_dir + file);
        //add to dictionary
        geometries[name] = geom_id;
    }

    //textures
    for (rapidjson::SizeType i = 0; i < json["textures"].Size(); i++) {
        //get values from json
        std::string name = json["textures"][i]["name"].GetString();
        std::string file = json["textures"][i]["file"].GetString();
        //load texture
        int tex_id = parseTexture(data_dir + file);
        //add to dictionary
        textures[name] = tex_id;
    }

    //shaders
    for (rapidjson::SizeType i = 0; i < json["shaders"].Size(); i++) {
        //get values from json
        std::string name = json["shaders"][i]["name"].GetString();
        std::string vertex = json["shaders"][i]["vertex"].GetString();
        std::string fragment = json["shaders"][i]["fragment"].GetString();
        //load shader

        Shader* new_shader = graphics_system.loadShader(vertex, fragment);
        new_shader->name = name;
        shaders[name] = new_shader->program;
    }

    //materials
    for (rapidjson::SizeType i = 0; i < json["materials"].Size(); i++) {
        //get values from json
        std::string name = json["materials"][i]["name"].GetString();
        //create material
        int mat_id = graphics_system.createMaterial();
        //shader_id is mandatory
        graphics_system.getMaterial(mat_id).shader_id = shaders[json["materials"][i]["shader"].GetString()];

        //optional properties
        //diffuse texture
        if (json["materials"][i].HasMember("diffuse_texture")) {
            std::string diffuse = json["materials"][i]["diffuse_texture"].GetString();
            graphics_system.getMaterial(mat_id).diffuse_map = textures[diffuse]; //assign texture id from material
        }
        //specular
        if (json["materials"][i].HasMember("specular")) {
            auto& json_spec = json["materials"][i]["specular"];
            graphics_system.getMaterial(mat_id).specular = lm::vec3(json_spec[0].GetFloat(), json_spec[1].GetFloat(), json_spec[2].GetFloat());
        }
        else
            graphics_system.getMaterial(mat_id).specular = lm::vec3(0, 0, 0); //no specular

        //ambient
        if (json["materials"][i].HasMember("ambient")) {
            auto& json_ambient = json["materials"][i]["ambient"];
            graphics_system.getMaterial(mat_id).ambient = lm::vec3(json_ambient[0].GetFloat(), json_ambient[1].GetFloat(), json_ambient[2].GetFloat());
        }
        else
            graphics_system.getMaterial(mat_id).ambient = lm::vec3(0.1f, 0.1f, 0.1f); //no specular

        //add to dictionary
        materials[name] = mat_id;
    }

    //lights
    for (rapidjson::SizeType i = 0; i < json["lights"].Size(); i++) {
        std::string light_name = json["lights"][i]["name"].GetString();
        auto json_lp = json["lights"][i]["position"].GetArray();
        auto json_lc = json["lights"][i]["color"].GetArray();

        int ent_light = ECS.createEntity(light_name);
        ECS.createComponentForEntity<Light>(ent_light);
        ECS.getComponentFromEntity<Transform>(ent_light).translate(json_lp[0].GetFloat(), json_lp[1].GetFloat(), json_lp[2].GetFloat());
        ECS.getComponentFromEntity<Light>(ent_light).color = lm::vec3(json_lc[0].GetFloat(), json_lc[1].GetFloat(), json_lc[2].GetFloat());
    }

    //entities
    for (rapidjson::SizeType i = 0; i < json["entities"].Size(); i++) {

        //json for entity
        auto& json_ent = json["entities"][i];

        //get name
        std::string json_name = "";
        if (json_ent.HasMember("name"))
            json_name = json_ent["name"].GetString();

        //get geometry and material ids - obligatory fields
        std::string json_geometry = json_ent["geometry"].GetString();
        std::string json_material = json_ent["material"].GetString();

        //transform - obligatory field
        auto jt = json_ent["transform"]["translate"].GetArray();
        auto jr = json_ent["transform"]["rotate"].GetArray();
        auto js = json_ent["transform"]["scale"].GetArray();

        //create entity
        int ent_id = ECS.createEntity(json_name);
        Mesh& ent_mesh = ECS.createComponentForEntity<Mesh>(ent_id);
        ent_mesh.geometry = geometries[json_geometry];
        ent_mesh.material = materials[json_material];

        //transform
        auto& ent_transform = ECS.getComponentFromEntity<Transform>(ent_id);

        //rotate
        //get rotation euler angles
        lm::vec3 rotate; rotate.x = jr[0].GetFloat(); rotate.y = jr[1].GetFloat(); rotate.z = jr[2].GetFloat();
        //create quaternion from euler angles
        lm::quat qR(rotate.x*DEG2RAD, rotate.y*DEG2RAD, rotate.z*DEG2RAD);
        //create matrix which represents these rotations
        lm::mat4 R; R.makeRotationMatrix(qR);
        //multiply transform by this matrix
        ent_transform.set(ent_transform * R);

        //scale
        ent_transform.scaleLocal(js[0].GetFloat(), js[1].GetFloat(), js[2].GetFloat());
        //translate
        ent_transform.translate(jt[0].GetFloat(), jt[1].GetFloat(), jt[2].GetFloat());

        if (json_ent["transform"].HasMember("parent")) {
            std::string json_parent = json_ent["transform"]["parent"].GetString();
            if (json_name == "" || json_parent == "") std::cerr << "ERROR: Parser: Either parent or child has no name";
            child_parent[json_name] = json_parent;
        }

        //optional fields below
        if (json_ent.HasMember("collider")) {
            std::string coll_type = json_ent["collider"]["type"].GetString();
            if (coll_type == "Box") {
                Collider& box_collider = ECS.createComponentForEntity<Collider>(ent_id);
                box_collider.collider_type = ColliderTypeBox;

                auto json_col_center = json_ent["collider"]["center"].GetArray();
                box_collider.local_center.x = json_col_center[0].GetFloat();
                box_collider.local_center.y = json_col_center[1].GetFloat();
                box_collider.local_center.z = json_col_center[2].GetFloat();

                auto json_col_halfwidth = json_ent["collider"]["halfwidth"].GetArray();
                box_collider.local_halfwidth.x = json_col_halfwidth[0].GetFloat();
                box_collider.local_halfwidth.y = json_col_halfwidth[1].GetFloat();
                box_collider.local_halfwidth.z = json_col_halfwidth[2].GetFloat();
            }
            ///TODO - Ray
        }
    }

    //now link hierarchy need to get transform id from parent entity,
    //and link to transform object from child entity
    for (std::pair<std::string, std::string> relationship : child_parent)
    {
        //get parent entity
        int parent_entity_id = ECS.getEntity(relationship.second);
        Entity& parent = ECS.entities[parent_entity_id];
        int parent_transform_id = parent.components[0]; //transform component is always in slot 0

        //get child transform
        Transform& transform_child = ECS.getComponentFromEntity<Transform>(relationship.first);

        //link child transform with parent id
        transform_child.parent = parent_transform_id;
    }

    return true;
}

// Method to parse the scene with new json structure
// No managers implemented, resources are loaded more than once!!

bool Parsers::parseScene(std::string filename, GraphicsSystem & graphics_system)
{
    // Set the json stream to be read
    std::ifstream json_file(filename);
    rapidjson::IStreamWrapper json_stream(json_file);
    rapidjson::Document json;
    json.ParseStream(json_stream);

    //check if its valid JSON
    if (json.HasParseError()) std::cerr << "JSON format is not valid!" << std::endl;
    if (!json.HasMember("scene")) { std::cerr << "JSON file is incomplete! Needs entry: scene" << std::endl; return false; }
    if (!json.HasMember("entities")) { std::cerr << "JSON file is incomplete! Needs entry: entities" << std::endl; return false; }

    printf("Parsing Scene Name = %s\n", json["scene"].GetString());

    // Create a default shader by now
    Shader* new_shader = graphics_system.loadShader("data/shaders/phong.vert", "data/shaders/phong.frag");
    new_shader->name = "phong";
    shaders["phong"] = new_shader->program;

    std::unordered_map<std::string, std::string> child_parent;

    for (rapidjson::SizeType i = 0; i < json["entities"].Size(); i++) {

        //get values from json
        rapidjson::Value & entity = json["entities"][i];
        int entity_id = parseEntity(entity, graphics_system);
        Entity ent = ECS.entities[entity_id];

        // Stablish a parent/children relationship if needed
        if (entity.HasMember("parent")) {
            std::string json_parent = entity["parent"].GetString();
            if (ent.name == "" || json_parent == "") std::cerr << "ERROR: Parser: Either parent or child has no name";
            child_parent[ent.name] = json_parent;
        }
    }

    //now link hierarchy need to get transform id from parent entity, 
    //and link to transform object from child entity
    for (std::pair<std::string, std::string> relationship : child_parent)
    {
        //get parent entity
        int parent_entity_id = ECS.getEntity(relationship.second);
        Entity& parent = ECS.entities[parent_entity_id];
        int parent_transform_id = parent.components[0];

        Transform& transform_child = ECS.getComponentFromEntity<Transform>(relationship.first);
        transform_child.parent = parent_transform_id;
    }

    return false;
}

int Parsers::parseEntity(rapidjson::Value & entity, GraphicsSystem & graphics_system)
{
    std::string name = "";
    if (entity.HasMember("name"))
        name = entity["name"].GetString();
    
    int ent_id = -1;
    if (entity.HasMember("prefab")) {
        /// In case of prefab entity, load the entity and then ignore it's default transform and name
        std::ifstream json_file(entity["prefab"].GetString());
        rapidjson::IStreamWrapper json_stream(json_file);
        rapidjson::Document json;
        json.ParseStream(json_stream);

        // Add support for multiple entities in prefab
        ent_id = parseEntity(json["entities"][0], graphics_system);
        ECS.entities[ent_id].name = name;
    }
    else {
        // Create the entity with the given name
        ent_id = ECS.createEntity(name);
    }

    //load transform component, mandatory field
    if (entity.HasMember("transform")) {

        auto& ent_transform = ECS.getComponentFromEntity<Transform>(ent_id);
        ent_transform.Load(entity, ent_id);
    }

    // Load render, geometry and materials
    if (entity.HasMember("render")) {

        int geo_id = Geometry::Load(graphics_system, entity, ent_id);
        int mat_id = Material::Load(graphics_system, entity, ent_id);

        Mesh& ent_mesh = ECS.createComponentForEntity<Mesh>(ent_id);
        ent_mesh.geometry = geo_id;
        ent_mesh.material = mat_id;
    }

    // Load collider parameters
    if (entity.HasMember("collider")) {

        Collider& collider = ECS.createComponentForEntity<Collider>(ent_id);
        collider.Load(entity, ent_id);       
    }

    // Add the light component
    if (entity.HasMember("light")) {;

        Light& light = ECS.createComponentForEntity<Light>(ent_id);
        light.Load(entity, ent_id);
    }

    // Custom components here!

    // Parse custom components here
    if (entity.HasMember("rotator")) {

        Rotator& rotator = ECS.createComponentForEntity<Rotator>(ent_id);
        rotator.Load(entity, ent_id);
    }

    if (entity.HasMember("tags")) {

        Tag& tag = ECS.createComponentForEntity<Tag>(ent_id);
        tag.Load(entity, ent_id);
    }

	if (entity.HasMember("moving")) {

		MovingPlatform& movable = ECS.createComponentForEntity<MovingPlatform>(ent_id);
		movable.Load(entity, ent_id);
	}

    ///TODO- Add any other component loading here!

    return ent_id;
}


/*int Parsers::parseEntity(rapidjson::Value & entity, GraphicsSystem & graphics_system)
{
    std::string name = "";
    if (entity.HasMember("name"))
        name = entity["name"].GetString();

    // Create the entity with the given name
    int ent_id = ECS.createEntity(name);

    if (entity.HasMember("prefab")) {
        /// In case of prefab entity, load the entity and then ignore it's default transform and name
        std::ifstream json_file(entity["prefab"].GetString());
        rapidjson::IStreamWrapper json_stream(json_file);
        rapidjson::Document json;
        json.ParseStream(json_stream);

        // Add support for multiple entities in prefab
        ent_id = parseEntity(json["entities"][0], graphics_system);
    }

    //load transform component, mandatory field
    if (entity.HasMember("transform")) {

        auto jt = entity["transform"]["translation"].GetArray();
        auto jr = entity["transform"]["rotation"].GetArray();
        auto js = entity["transform"]["scale"].GetArray();

        //transform
        auto& ent_transform = ECS.getComponentFromEntity<Transform>(ent_id);

        lm::vec3 rotate; rotate.x = jr[0].GetFloat(); rotate.y = jr[1].GetFloat(); rotate.z = jr[2].GetFloat();
        lm::quat qR(rotate.x*DEG2RAD, rotate.y*DEG2RAD, rotate.z*DEG2RAD);
        lm::mat4 R;
        R.rotateLocal(rotate.x*DEG2RAD, lm::vec3(1, 0, 0));
        R.rotateLocal(rotate.y*DEG2RAD, lm::vec3(0, 1, 0));
        R.rotateLocal(rotate.z*DEG2RAD, lm::vec3(0, 0, 1));
        ent_transform.set(ent_transform * R);
        ent_transform.scaleLocal(js[0].GetFloat(), js[1].GetFloat(), js[2].GetFloat());
        ent_transform.translate(jt[0].GetFloat(), jt[1].GetFloat(), jt[2].GetFloat());
    }

    if (entity.HasMember("render")) {

        // Parse mesh and materials
        // By now, only one single material per mesh
        auto jmesh = entity["render"]["mesh"].GetString();
        auto jmat = entity["render"]["materials"].GetArray();
        std::string mat_name = jmat[0].GetString();

        std::ifstream json_file(jmat[0].GetString());
        rapidjson::IStreamWrapper json_stream(json_file);
        rapidjson::Document json_material;
        json_material.ParseStream(json_stream);

        int geo_id;
        if (geometries.find(jmesh) == geometries.end()) { 
            geo_id = graphics_system.createGeometryFromFile(jmesh); 
            geometries[jmesh] = geo_id; 
        }
        else { 
            geo_id = geometries[jmesh]; 
        }

        int mat_id = graphics_system.createMaterial();

        // Load shader data from technique
        // Hardcoded by now until technique source file is done.
        graphics_system.getMaterial(mat_id).shader_id = shaders["phong"];
        if (json_material.HasParseError()) std::cerr << "JSON format is not valid!" << std::endl;

        if (json_material["textures"].HasMember("diffuse")) {
            std::string tx_diff = json_material["textures"]["diffuse"].GetString();
            int tex_id;
            if (textures.find(tx_diff) == textures.end()) { tex_id = parseTexture(tx_diff); textures[tx_diff] = tex_id; }
            else { tex_id = textures[tx_diff]; }

            graphics_system.getMaterial(mat_id).diffuse_map = tex_id; //assign texture id from material
        }

        if (json_material["textures"].HasMember("specular")) {
            std::string tx_spec = json_material["textures"]["specular"].GetString();
            int tex_id = parseTexture(tx_spec);
            //graphics_system.getMaterial(mat_id).specular = lm::vec3(json_spec[0].GetFloat(), json_spec[1].GetFloat(), json_spec[2].GetFloat());
        }
        else {
            graphics_system.getMaterial(mat_id).specular = lm::vec3(0, 0, 0); //no specular
        }

        if (json_material["textures"].HasMember("ambient")) {
            std::string tx_amb = json_material["textures"]["ambient"].GetString();
            int tex_id = parseTexture(tx_amb);
            //graphics_system.getMaterial(mat_id).ambient = lm::vec3(json_ambient[0].GetFloat(), json_ambient[1].GetFloat(), json_ambient[2].GetFloat());
        }
        else {
            graphics_system.getMaterial(mat_id).ambient = lm::vec3(0.1f, 0.1f, 0.1f); //small ambient
        }


        Mesh& ent_mesh = ECS.createComponentForEntity<Mesh>(ent_id);
        ent_mesh.geometry = geo_id;
        ent_mesh.material = mat_id;
    }

    if (entity.HasMember("collider")) {

        std::string coll_type = entity["collider"]["type"].GetString();
        if (coll_type == "box") {
            Collider& box_collider = ECS.createComponentForEntity<Collider>(ent_id);
            box_collider.collider_type = ColliderTypeBox;

            auto json_col_center = entity["collider"]["center"].GetArray();
            box_collider.local_center.x = json_col_center[0].GetFloat();
            box_collider.local_center.y = json_col_center[1].GetFloat();
            box_collider.local_center.z = json_col_center[2].GetFloat();

            auto json_col_halfwidth = entity["collider"]["halfwidth"].GetArray();
            box_collider.local_halfwidth.x = json_col_halfwidth[0].GetFloat();
            box_collider.local_halfwidth.y = json_col_halfwidth[1].GetFloat();
            box_collider.local_halfwidth.z = json_col_halfwidth[2].GetFloat();
        }
        ///TODO - Ray
    }

    // Add the light component
    if (entity.HasMember("light")) {
        auto json_lc = entity["light"]["color"].GetArray();

        ECS.createComponentForEntity<Light>(ent_id);
        ECS.getComponentFromEntity<Light>(ent_id).color = lm::vec3(json_lc[0].GetFloat(), json_lc[1].GetFloat(), json_lc[2].GetFloat());
    }

    // Parse custom components here
    if (entity.HasMember("rotator")) {
        auto json_sp = entity["rotator"]["speed"].GetFloat();
        auto json_axis = entity["rotator"]["axis"].GetArray();

        ECS.createComponentForEntity<Rotator>(ent_id);
        ECS.getComponentFromEntity<Rotator>(ent_id).speed = json_sp;
        ECS.getComponentFromEntity<Rotator>(ent_id).axis = lm::vec3(json_axis[0].GetFloat(), json_axis[1].GetFloat(), json_axis[2].GetFloat());
    }

    if (entity.HasMember("tags")) {
        auto& json_tags = entity["tags"].GetArray();
        std::vector<std::string> tags;
        for (auto& p : json_tags)
            tags.push_back(p.GetString());

        ECS.createComponentForEntity<Tag>(ent_id);
        ECS.getComponentFromEntity<Tag>(ent_id).tags = tags;
    }
    ///TODO- Add any other component loading here!

    return ent_id;
}
*/