/*#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include "binaryReader.h"
#include "Vector.h"
class CoDTexture {
public:
    std::string Name;
    std::string Type;

    CoDTexture(binaryReader& reader) {
        Name = reader.readString();
        Type = reader.readString();
    }
};

std::unordered_map<int, std::string> CoDVersions = {
    {0, "modern_warfare"},
    {1, "world_at_war"},
    {2, "modern_warfare_2"},
    {3, "black_ops"},
    {4, "modern_warfare_3"},
    {5, "black_ops_2"},
    {6, "ghosts"},
    {7, "advanced_warfare"},
    {8, "black_ops_3"},
    {9, "modern_warfare_rm"},
    {10, "infinite_warfare"},
    {11, "world_war_2"},
    {12, "black_ops_4"},
    {13, "modern_warfare_4"},
    {14, "modern_warfare_2_rm"},
    {15, "black_ops_5"}
};

std::unordered_map<int, std::string> CoDMaterialSettings = {
    {0, "colorTint"},
    {1, "detailScale"},
    {2, "detailScale1"},
    {3, "detailScale2"},
    {4, "detailScale3"},
    {5, "specColorTint"},
    {6, "rowCount"},
    {7, "columnCount"},
    {8, "imageTime"}
};


std::unordered_map<int, std::string> CoDLights = {
    {1, "SUN"},
    {2, "SPOT"},
    {3, "SPOT"},
    {4, "SPOT"},
    {5, "POINT"}
};

class CoDLight {
    std::string Type;
    Vector3 Origin;
    Vector3 Direction;
    Vector4 Color;
    Vector3 Angles;
    float Radius;
    float CosHalfFovOuter;
    float CosHalfFovInner;
    float dAttenuation;
public:
    CoDLight(binaryReader& reader) {
        Type = CoDLights.at(reader.readBytes(1).at(0));
        Origin = reader.readVector3();
        Direction = reader.readVector3();
        Angles = reader.readVector3();
        Color = reader.readVector4();
        Radius = reader.readFloat();
        CosHalfFovOuter = reader.readFloat();
        CosHalfFovInner = reader.readFloat();
        dAttenuation = reader.readFloat();
    }
};

class CoDModelInstance {
public:
    std::string Name;
    Vector3 Position;
    Vector4 Rotation;
    Vector3 Rotation_euler;
    uint8_t Rotation_mode;
    Vector3 Scale;
    CoDModelInstance(binaryReader& reader) {
        Name = reader.readString();
        std::cout << "Model Name: " << Name << std::endl;
        Position = reader.readVector3();
        Rotation_mode = reader.readBytes(1).at(0);
        // Quat Rotation 
        if (Rotation_mode == 0) {
            float QuatRoat[4] = { reader.readFloat(),reader.readFloat(), reader.readFloat(), reader.readFloat() };
            Rotation = Vector4(QuatRoat[3], QuatRoat[0], QuatRoat[1], QuatRoat[2]);

        }
        else if (Rotation_mode == 1) {
            auto tempRoat = reader.readVector3();
            tempRoat.ToRad();
            Rotation_euler = tempRoat;
        }

        Scale = reader.readVector3();
    }
};


class CoDMaterial {
public:
    std::string Name;
    std::string TechSet;
    int SortKey;
    std::vector<CoDTexture> Textures;
    std::unordered_map<std::string, char*> Settings;

    CoDMaterial(binaryReader& reader) {
        Name = reader.readString();
        TechSet = reader.readString();
        SortKey = reader.readBytes(1).at(0);
        std::cout << "Material Name: " << Name << std::endl;
        auto imageCountVec = reader.readBytes(1);
        auto imageCount = (imageCountVec.at(0));
        std::cout << "Image Count: " << (int)imageCount << std::endl;
        for (int i = 0; i < imageCount; i++) {
            Textures.push_back(CoDTexture(reader));
        }
        std::cout << "Done with images" << std::endl;
        auto settingCountVec = reader.readBytes(1);
        auto settingCount = (settingCountVec.at(0));
        std::cout << "Setting Count: " << settingCount << std::endl;
        for (int i = 0; i < settingCount; i++) {
            auto setting = CoDMaterialSettings.at(reader.readBytes(1).at(0));
            auto value = reader.readString();
            Settings[setting] = value;
        }
    }
};



class CoDSurface {
    std::string Name;
    std::vector<std::vector<int>> Faces;
    std::vector<std::string> Materials;
public:
    CoDSurface(binaryReader& reader) {
        Name = reader.readString();
        auto materialCount = reader.readInt();
        for (int i = 0; i < materialCount; i++) {
            auto materialName = reader.readString();
            Materials.push_back(materialName);
        }
        auto faceCount = reader.readInt();
        for (int f = 0; f < faceCount; f++) {
            std::vector<int> vec{};
            for (int x = 0; x < 3; x++) {
                auto vertex_index = reader.readInt();
                vec.push_back(vertex_index);
            }
            Faces.push_back(vec);
        }
    }
};

class CodMesh {
private:
    std::string Name;
    std::vector<Vector> Vertices;
    std::vector<Vector> Normals;
    std::vector <std::vector<vec2_t>> UVs;
    std::vector<Color> Colors;
    std::vector<CoDSurface> Surfaces;

    bool isXModel;
public:
    CodMesh(binaryReader& reader) {

        Name = reader.readString();

        isXModel = reader.readBool();
        std::cout << Name << ' ' << isXModel << std::endl;
        auto vertCount = reader.readInt();
        for (int i = 0; i < vertCount; i++) {
            auto x = reader.readFloat();
            auto y = reader.readFloat();
            auto z = reader.readFloat();
            Vertices.push_back(Vector{ x, y, z });
        }

        auto normCount = reader.readInt();
        for (int i = 0; i < normCount; i++) {
            auto x = reader.readFloat();
            auto y = reader.readFloat();
            auto z = reader.readFloat();
            Normals.push_back(Vector{ x, y, z });
        }

        auto uvCount = reader.readInt();
        for (int i = 0; i < uvCount; i++) {
            auto uvSet_count = reader.readInt();
            std::vector<vec2_t> uvSet;
            for (int x = 0; x < uvSet_count; x++) {
                uvSet.push_back(vec2_t{ reader.readFloat(), reader.readFloat() });
            }
            UVs.push_back(uvSet);
        }

        auto colorCount = reader.readInt();
        for (int i = 0; i < colorCount; i++) {

            auto r = static_cast<float>(reader.readBool()) / 255.0f;
            auto g = static_cast<float>(reader.readBool()) / 255.0f;
            auto b = static_cast<float>(reader.readBool()) / 255.0f;
            auto a = static_cast<float>(reader.readBool()) / 255.0f;
            auto color = Color{ r, g, b, a };
            Colors.push_back(color);
        }

        auto surfaceCount = reader.readInt();
        for (int i = 0; i < surfaceCount; i++) {
            auto surface = CoDSurface(reader);
            Surfaces.push_back(surface);
        }


    }
};

class CodMap {
public:
    std::vector<CodMesh> meshes;
    std::string Name;
    std::string SkyboxName;
    std::unordered_map<std::string, CoDMaterial> Materials;
    std::vector<CoDModelInstance> ModelInstances;
    std::vector<CoDLight> Lights;
    std::string Version;

    CodMap(std::string fileName) {
        auto reader = binaryReader(fileName);
        auto nVersion = reader.readBytes(3);
        auto file_version = reader.readBytes(2);
        Version = CoDVersions[file_version.at(1)];
        std::cout << Version << std::endl;

        if (nVersion[0] != 'C' || nVersion[1] != '2' || nVersion[2] != 'M')
        {
            std::cout << "Invalid file format" << std::endl;
            return;
        }

        Name = reader.readString();
        std::cout << "Map Name: " << Name << std::endl;
        SkyboxName = reader.readString();
        std::cout << "Skybox Name: " << SkyboxName << std::endl;

        auto objCount = reader.readInt();
        std::cout << objCount << std::endl;
        for (int i = 0; i < objCount; i++) {
            auto mesh = CodMesh(reader);
            meshes.push_back(mesh);
        }

        auto materialCount = reader.readInt();
        std::cout << materialCount << std::endl;
        for (int i = 0; i < materialCount; i++) {
            auto material = CoDMaterial(reader);
            Materials.insert({ material.Name, material });
        }
        auto instanceCount = reader.readInt();
        std::cout << instanceCount << std::endl;
        for (int i = 0; i < instanceCount; i++) {
            auto instance = CoDModelInstance(reader);
            ModelInstances.push_back(instance);
        }

        auto lightCount = reader.readInt();
        std::cout << lightCount << std::endl;
        for (int i = 0; i < lightCount; i++) {
            auto light = CoDLight(reader);
            Lights.push_back(light);
        }
    }

}; */