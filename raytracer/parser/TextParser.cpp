#include "TextParser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "SceneData.h"
#include "TriangleData.h"

void TextParser::matrixTransform(float* values)
{
    mat4 transform = transforms.top();
    vec4 valvec = vec4(values[0], values[1], values[2], values[3]);
    vec4 newval = transform * valvec;
    for (int i = 0; i < 4; i++) values[i] = newval[i];
}

void TextParser::rightMultiply(const mat4& matrix)
{
    mat4& T = transforms.top();
    T = T * matrix;
}

bool TextParser::readValues(std::stringstream& s, const int numvals, float* values) {
    for (int i = 0; i < numvals; i++) {
        s >> values[i];
        if (s.fail()) {
            std::cout << "Failed reading value " << i << " will skip\n";
            return false;
        }
    }
    return true;
}

SceneData* TextParser::parse(const char* filename) {
    std::string str;
    std::ifstream in;
    in.open(filename);
    if (in.is_open()) {
        initInstructionsMap();
        sceneData = new SceneData();
        transforms.push(mat4(1.0));  // identity

        getline(in, str);
        while (in) {
            parseLine(str);
            getline(in, str);
        }
    } else {
        std::cerr << "Unable to Open Input Data File " << filename << "\n";
        throw 2;
    }
    return sceneData;
};

void TextParser::parseLine(const std::string& line)
{
    if (isEmptyOrComment(line))
    {
        std::stringstream s(line);
        std::string instructionName;
        s >> instructionName;
        auto it = instructionsMap.find(instructionName);
        if (it != instructionsMap.end()) {
            ParseFunc func = it->second;
            (this->*func)(s);
        }

        /*if (cmd == "directional" || cmd == "point")
        {
            validinput = readValuess(s, 6, values);
            if (validinput)
            {
                lights.push_back(LightData(cmd == "directional", values));
            }
        }
        else if (cmd == "ambient")
        {
            validinput = readValues(s, 3, values); // colors
            if (validinput)
            {
                for (i = 0; i < 3; i++)
                {
                    ambient[i] = values[i];
                }
            }
        } else if (cmd == "diffuse")
        {
            validinput = readValues(s, 3, values);
            if (validinput)
            {
                for (i = 0; i < 3; i++)
                {
                    diffuse[i] = values[i];
                }
            }
        } else if (cmd == "specular")
        {
            validinput = readValues(s, 3, values);
            if (validinput)
            {
                for (i = 0; i < 3; i++)
                {
                    specular[i] = values[i];
                }
            }
        } else if (cmd == "emission")
        {
            validinput = readValues(s, 3, values);
            if (validinput)
            {
                for (i = 0; i < 3; i++)
                {
                    emission[i] = values[i];
                }
            }
        } else if (cmd == "shininess")
        {
            validinput = readValues(s, 1, values);
            if (validinput)
            {
                shininess = values[0];
            }
        }
        else if (cmd == "sphere")
        {
            validinput = readValues(s, 1, values);
            if (validinput)
            {
                Object* obj = &(objects[numobjects]);
                obj->size = values[0];

                for (i = 0; i < 4; i++)
                {
                    (obj->ambient)[i] = ambient[i];
                    (obj->diffuse)[i] = diffuse[i];
                    (obj->specular)[i] = specular[i];
                    (obj->emission)[i] = emission[i];
                }
                obj->shininess = shininess;

                obj->transform = transfstack.top();
            }
        }
        else if (cmd == "translate")
        {
            validinput = readValues(s, 3, values);
            if (validinput)
            {
                const mat4& translateM = Transform::translate(values[0], values[1], values[2]);
                rightmultiply(translateM, transfstack);
            }
        }
        else if (cmd == "scale")
        {
            validinput = readValues(s, 3, values);
            if (validinput)
            {
                const mat4& scaleM = Transform::scale(values[0], values[1], values[2]);
                rightmultiply(scaleM, transfstack);
            }
        }
        else if (cmd == "rotate")
        {
            validinput = readValues(s, 4, values);
            if (validinput)
            {
                const mat4& rotateM = mat4(Transform::rotate(values[3], vec3(values[0], values[1], values[2])));
                rightmultiply(rotateM, transfstack);
            }
        }
        else if (cmd == "pushTransform")
        {
            transfstack.push(transfstack.top());
        } else if (cmd == "popTransform")
        {
            if (transfstack.size() <= 1)
            {
                std::cerr << "Stack has no elements.  Cannot Pop\n";
            } else
            {
                transfstack.pop();
            }
        }

        else
        {
            std::cerr << "Unknown Command: " << cmd << " Skipping \n";
        }*/
    }
}

bool TextParser::isEmptyOrComment(const std::string& line) const {
    return (line.find_first_not_of(" \t\r\n") != std::string::npos) && (line[0] != '#');
}

void TextParser::initInstructionsMap() {
    instructionsMap["size"] = &TextParser::parseSize;
    instructionsMap["maxdepth"] = &TextParser::parseMaxdepth;
    instructionsMap["output"] = &TextParser::parseOutput;
    instructionsMap["camera"] = &TextParser::parseCamera;
    instructionsMap["sphere"] = &TextParser::parseSphere;
    instructionsMap["maxverts"] = &TextParser::parseMaxverts;
    instructionsMap["maxvertnorms"] = &TextParser::parseMaxvertnorms;
    instructionsMap["vertex"] = &TextParser::parseVertex;
    instructionsMap["vertexnormal"] = &TextParser::parseVertexnormal;
    instructionsMap["tri"] = &TextParser::parseTri;
    instructionsMap["trinormal"] = &TextParser::parseTrinormal;
    instructionsMap["translate"] = &TextParser::parseTranslate;
    instructionsMap["rotate"] = &TextParser::parseRotate;
    instructionsMap["scale"] = &TextParser::parseScale;
    instructionsMap["pushTransform"] = &TextParser::parsePushTransform;
    instructionsMap["popTransform"] = &TextParser::parsePopTransform;
    instructionsMap["directional"] = &TextParser::parseDirectional;
    instructionsMap["point"] = &TextParser::parsePoint;
    instructionsMap["attenuation"] = &TextParser::parseAttenuation;
    instructionsMap["ambient"] = &TextParser::parseAmbient;
    instructionsMap["diffuse"] = &TextParser::parseDiffuse;
    instructionsMap["specular"] = &TextParser::parseSpecular;
    instructionsMap["shininess"] = &TextParser::parseShininess;
    instructionsMap["emission"] = &TextParser::parseEmission;
}

void TextParser::parseSize(std::stringstream& s) {
    readValues(s, 2, values);
    sceneData->width = (int) values[0];
    sceneData->height = (int) values[1];
}

void TextParser::parseMaxdepth(std::stringstream& s) {
    
}

void TextParser::parseOutput(std::stringstream& s) {
    
}

void TextParser::parseCamera(std::stringstream& s) {
    readValues(s, 10, values);
    sceneData->camera.lookFrom[0] = values[0];
    sceneData->camera.lookFrom[1] = values[1];
    sceneData->camera.lookFrom[2] = values[2];
    sceneData->camera.lookAt[0] = values[3];
    sceneData->camera.lookAt[1] = values[4];
    sceneData->camera.lookAt[2] = values[5];
    sceneData->camera.up[0] = values[6];
    sceneData->camera.up[1] = values[7];
    sceneData->camera.up[2] = values[8];
    sceneData->camera.fovy = values[9];
}

void TextParser::parseSphere(std::stringstream& s) {

}

void TextParser::parseMaxverts(std::stringstream& s) {
    readValues(s, 1, values);
    sceneData->maxVerticies = (int) values[0];
}

void TextParser::parseMaxvertnorms(std::stringstream& s) {

}

void TextParser::parseVertex(std::stringstream& s) {
    readValues(s, 3, values);
    sceneData->verticies.push_back(glm::vec3(values[0], values[1], values[2]));
}

void TextParser::parseVertexnormal(std::stringstream& s) {

}

void TextParser::parseTri(std::stringstream& s) {
    readValues(s, 3, values);
    TriangleData* tri = new TriangleData((unsigned) values[0],(unsigned)  values[1],(unsigned)  values[2]);
    *tri->ambient = *ambient;
    sceneData->triangles.push_back(tri);
}

void TextParser::parseTrinormal(std::stringstream& s) {

}

void TextParser::parseTranslate(std::stringstream& s) {

}

void TextParser::parseRotate(std::stringstream& s) {

}

void TextParser::parseScale(std::stringstream& s) {

}

void TextParser::parsePushTransform(std::stringstream& s) {

}

void TextParser::parsePopTransform(std::stringstream& s) {

}

void TextParser::parseDirectional(std::stringstream& s) {

}

void TextParser::parsePoint(std::stringstream& s) {

}

void TextParser::parseAttenuation(std::stringstream& s) {

}

void TextParser::parseAmbient(std::stringstream& s) {
    readValues(s, 3, values);
    ambient[0] = values[0];
    ambient[1] = values[1];
    ambient[2] = values[2];
}

void TextParser::parseDiffuse(std::stringstream& s) {

}

void TextParser::parseSpecular(std::stringstream& s) {

}

void TextParser::parseShininess(std::stringstream& s) {

}

void TextParser::parseEmission(std::stringstream& s) {

}
