#pragma once

#include "../Core/CoreDefs.h"
#include "../Scene/Entity.h"
#include "../Scene/SceneGraph.h"

#include "../Dependencies/Miscellaneous/json.hpp"

using nlohmann::json;

ION_NAMESPACE_BEGIN

/*
//////////////////////////////////////////////////////////////////////////

void to_json(json& _json, const Vector& _input);
void from_json(const json& _json, Vector& _output);

void to_json(json& _json, const Quaternion& _input);
void from_json(const json& _json, Quaternion& _output);

void to_json(json& _json, const Matrix& _input);
void from_json(const json& _json, Matrix& _output);
*/

//////////////////////////////////////////////////////////////////////////

void to_json(json& _json, const Index& _input);
void from_json(const json& _json, Index& _output);

void to_json(json& _json, const VertexPlain& _input);
void from_json(const json& _json, VertexPlain& _output);
void to_json(json& _json, const VertexColored& _input);
void from_json(const json& _json, VertexColored& _output);
void to_json(json& _json, const VertexNormal& _input);
void from_json(const json& _json, VertexNormal& _output);
void to_json(json& _json, const VertexUV& _input);
void from_json(const json& _json, VertexUV& _output);
void to_json(json& _json, const VertexSimple& _input);
void from_json(const json& _json, VertexSimple& _output);
void to_json(json& _json, const Vertex& _input);
void from_json(const json& _json, Vertex& _output);
void to_json(json& _json, const VertexMorphTarget& _input);
void from_json(const json& _json, VertexMorphTarget& _output);
//////////////////////////////////////////////////////////////////////////

void to_json(json& _json, const ObjectHandler& _input);
void from_json(const json& _json, ObjectHandler& _output);

//////////////////////////////////////////////////////////////////////////

std::string Serialize(const ObjectHandler& _input);

ION_NAMESPACE_END



namespace nlohmann {
    template <>
    struct adl_serializer<Vector> {
        static void to_json(json& j, const Vector& opt)
        {
            std::ostringstream oss;
            oss << opt;
            std::string value = oss.str();

            j = value;
        }

        static void from_json(const json& j, Vector& opt) 
        {
            //opt = j.get<Vector>();
        }

        static void to_json(json& j, const Quaternion& opt) 
        {
            std::ostringstream oss;
            oss << opt;
            std::string value = oss.str();

            j = value;
        }

        static void from_json(const json& j, Quaternion& opt)
        {
            //opt = j.get<Vector>();
        }

        static void to_json(json& j, const Matrix& opt) 
        {
            std::ostringstream oss;
            oss << opt;
            std::string value = oss.str();

            j = value;
        }

        static void from_json(const json& j, Matrix& opt) 
        {
            //opt = j.get<Vector>();
        }
    };
}



/*
// EXAMPLE json.hpp integration!

#include <iostream>
#include <string>

struct RoomData
{
    int id;
    std::string name;
    int maxPlayers;
    int timePerQuestion;
    int isActive;
};

using nlohmann::json;

void to_json(json& j, const RoomData& r) {
    j = json{
        { "id", r.id },
        { "name", r.name },
        { "maxPlayers", r.maxPlayers },
        { "timePerQuestion", r.timePerQuestion },
        { "isActive", r.isActive }
    };
}

void from_json(const json& j, RoomData& r) {
    r.id = j.at("id").get<int>();
    r.name = j.at("name").get<std::string>();
    r.maxPlayers = j.at("maxPlayers").get<int>();
    r.timePerQuestion = j.at("timePerQuestion").get<int>();
    r.isActive = j.at("isActive").get<int>();
}

std::string serialize(std::vector<RoomData> roomData)
{
    json j(roomData);
    std::string jsonArray = j.dump();
    return jsonArray;
}

int main()
{
    std::vector<RoomData> rm{ { 1, "first", 2, 3, 1 },{ 2, "second", 5, 6, 4 } };
    std::cout << serialize(rm);
}
*/