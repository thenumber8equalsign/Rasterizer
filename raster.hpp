#pragma once

#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <optional>
#include <fstream>
#include <limits.h>
#include <unistd.h>

namespace Raster {
    // Much to my dismay, we can't force the user into radians despite them being infinitely better than degrees
    inline __attribute__((always_inline)) float toRadians(float degrees) {
        return degrees*M_PIf/180.0f;
    }
    inline __attribute__((always_inline)) float toDegrees(float rad) {
        return rad*180.0f/M_PIf;
    }

    inline __attribute__((always_inline)) float m(float a, float b) {
        return a-b*floor(a/b);
    }
    inline __attribute__((always_inline)) float normalizeAngle(float rads) {
        return m(rads,2*M_PIf);
    }

    #ifdef __linux__
    inline __attribute__((always_inline)) std::string getExecutablePath() {
        char path[PATH_MAX+1];

        ssize_t len = readlink("/proc/self/exe", path, PATH_MAX);
        if (len == -1) {
            throw std::runtime_error("readlink fail");
        }
        path[len]= '\0';
        return std::string(path);
    }
    #else
    #error "Linux only, I was too lazy to make it work on windows/mac/bsd/other"
    #endif

    // Structures
    struct float2 {
        float x, y;

        inline __attribute__((always_inline)) float2 operator*(float scalar) const {
            return {x*scalar, y*scalar};
        }
        inline __attribute__((always_inline)) float2 operator/(float scalar) const {
            return {x/scalar, y/scalar};
        }
        inline __attribute__((always_inline)) float2& operator*=(float scalar) {
            x *=scalar;
            y *=scalar;
            return *this;
        }
        inline __attribute__((always_inline)) float2 operator+(const float2& other) const {
            return {x+other.x, y+other.y};
        }
        inline __attribute__((always_inline)) float2 operator-() const {
            return {-x, -y};
        }
        inline __attribute__((always_inline)) float2 operator-(const float2& other) const {
            return {x-other.x, y-other.y};
        }
        inline __attribute__((always_inline)) float2& operator+=(const float2& other) {
            x+=other.x;
            y+=other.y;
            return *this;
        }
        inline __attribute__((always_inline)) float2& operator-=(const float2& other) {
            x-=other.x;
            y-=other.y;
            return *this;
        }

        static inline __attribute__((always_inline)) float2 perpendicular(const float2& a) {
            return {-a.y, a.x};
        }

        static inline __attribute__((always_inline)) float dot(const float2& a, const float2& b) {
            return a.x*b.x+a.y*b.y;
        }
    };
    typedef struct float2 float2;

    struct float3 {
        float x, y, z;

        inline __attribute__((always_inline)) float3 operator*(float scalar) const {
            return {x*scalar, y*scalar, z*scalar};
        }
        inline __attribute__((always_inline)) float3 operator/(float scalar) const {
            return {x/scalar, y/scalar, z/scalar};
        }
        inline __attribute__((always_inline)) float3 operator+(const float3& other) const {
            return {x+other.x, y+other.y, z+other.z};
        }
        inline __attribute__((always_inline)) float3 operator-() const {
            return {-x, -y, -z};
        }
        inline __attribute__((always_inline)) float3 operator-(const float3& other) const {
            return {x-other.x, y-other.y, z-other.z};
        }
        inline __attribute__((always_inline)) float3& operator+=(const float3& other) {
            x+=other.x;
            y+=other.y;
            z+=other.z;
            return *this;
        }
        inline __attribute__((always_inline)) float3& operator-=(const float3& other) {
            x-=other.x;
            y-=other.y;
            z-=other.z;
            return *this;
        }

        inline __attribute__((always_inline)) bool operator==(const float3& other) {
            return x == other.x && y == other.y && z == other.z;
        }

        inline __attribute__((always_inline)) float2 dropZ() const {
            return {x, y};
        }

        inline __attribute__((always_inline)) float3 normalize() const {
            const float mag = std::hypotf(x, std::hypotf(y, z));
            return {x/mag, y/mag, z/mag};
        }

        static inline __attribute__((always_inline)) float dot(float3 a, float3 b) {
            return a.x*b.x + a.y*b.y + a.z*b.z;
        }
    };
    typedef struct float3 float3;

    constexpr float3 float3_forward{0,0,1};
    constexpr float3 float3_right{1,0,0};
    constexpr float3 float3_up{0,1,0};
    constexpr float3 float3_zero{0,0,0};

    inline __attribute__((always_inline)) float3 operator*(float scalar, const float3& f3) {
        return {scalar*f3.x, scalar*f3.y, scalar*f3.z};
    }

    inline __attribute__((always_inline)) float3 operator/(float scalar, const float3& f3) {
        return {scalar/f3.x, scalar/f3.y, scalar/f3.z};
    }

    inline __attribute__((always_inline)) std::ostream& operator<<(std::ostream& s, const float3& f3) {
        s << "float3{ x=" << f3.x << ", y=" << f3.y << ", z=" << f3.z << " }";
        return s;
    }
    inline __attribute__((always_inline)) std::ostream& operator<<(std::ostream& s, const float2& f2) {
        s << "float2{ x=" << f2.x << ", y=" << f2.y << " }";
        return s;
    }

    struct triangle {
        float2 a, b, c;
    };
    typedef struct triangle triangle;

    struct triangle3D {
        float3 a, b, c;
    };
    typedef struct triangle3D triangle3D;

    inline __attribute__((always_inline)) std::ostream& operator<<(std::ostream& s, const triangle3D& tri) {
        s << "triangle3D{ a=" << tri.a << ", b=" << tri.b << ", c=" << tri.c << " }";
        return s;
    }

    class Transform {
        public:
            Transform(const float3& pos, const float yaw, const float pitch, const float roll);
            inline __attribute__((always_inline)) void updateBasisVectors();
            inline __attribute((always_inline)) void fetchBasisVectors(float3* ihat, float3* jhat, float3* khat) {
                *ihat = this->iHat;
                *jhat = this->jHat;
                *khat = this->kHat;
            }
            inline __attribute__((always_inline)) void fetchInverseBasisVectors(float3* ihat, float3* jhat, float3* khat) {
                *ihat = this->iHatInv;
                *jhat = this->jHatInv;
                *khat = this->kHatInv;
            }

            Transform* parent = nullptr;
            float3 position;

            inline __attribute__((always_inline)) void setYaw(float yaw);
            inline __attribute__((always_inline)) void setPitch(float pitch);
            inline __attribute__((always_inline)) void setRoll(float roll);

            inline __attribute__((always_inline)) void incYaw(float inc);
            inline __attribute__((always_inline)) void incPitch(float inc);
            inline __attribute__((always_inline)) void incRoll(float inc);

            inline __attribute__((always_inline)) void setRotation(float yaw, float pitch, float roll);

            inline __attribute__((always_inline)) float getYaw() {return toDegrees(yaw);};
            inline __attribute__((always_inline)) float getPitch() {return toDegrees(pitch);};
            inline __attribute__((always_inline)) float getRoll() {return toDegrees(roll);};

            inline __attribute__((always_inline)) static float3 transformVector(const float3& iHat, const float3& jHat, const float3& kHat, const float3& v) {
                return v.x * iHat + v.y * jHat + v.z * kHat;
            }
        private:
            float yaw, pitch, roll;
            float3 iHat, jHat, kHat;
            //     right, up , forward
            float3 iHatInv, jHatInv, kHatInv;

            inline __attribute__((always_inline)) void computeBasisVectors(float3* iHat, float3* jHat, float3* kHat);
            inline __attribute__((always_inline)) void computeInverseBasisVectors(float3* iHat, float3* jHat, float3* kHat);
    };

    Transform::Transform(const float3& pos, const float yaw, const float pitch, const float roll) {
        this->position = pos;
        setRotation(yaw, pitch, roll);
        updateBasisVectors();
    }

    inline __attribute__((always_inline)) void Transform::setRotation(float yaw, float pitch, float roll) {
        this->yaw = normalizeAngle(toRadians(yaw));
        this->pitch = normalizeAngle(toRadians(pitch));
        this->roll = normalizeAngle(toRadians(roll));

        updateBasisVectors();
    }

    inline __attribute__((always_inline)) void Transform::setYaw(float yaw) {
        this->yaw = normalizeAngle(toRadians(yaw));
        updateBasisVectors();
    }

    inline __attribute__((always_inline)) void Transform::setPitch(float pitch) {
        this->pitch = normalizeAngle(toRadians(pitch));
        updateBasisVectors();
    }

    inline __attribute__((always_inline)) void Transform::setRoll(float roll) {
        this->roll = normalizeAngle(toRadians(roll));
        updateBasisVectors();
    }

    inline __attribute__((always_inline)) void Transform::incYaw(float inc) {
        yaw = normalizeAngle(yaw + toRadians(inc));
        updateBasisVectors();
    }

    inline __attribute__((always_inline)) void Transform::incPitch(float inc) {
        pitch = normalizeAngle(pitch + toRadians(inc));
        updateBasisVectors();
    }

    inline __attribute__((always_inline)) void Transform::incRoll(float inc) {
        roll = normalizeAngle(roll + toRadians(inc));
        updateBasisVectors();
    }

    inline __attribute__((always_inline)) void Transform::updateBasisVectors() {
        computeBasisVectors(&iHat, &jHat, &kHat);
        computeInverseBasisVectors(&iHatInv, &jHatInv, &kHatInv);
    }

    inline __attribute__((always_inline)) void Transform::computeBasisVectors(float3* iHat, float3* jHat, float3* kHat) {
        // Yaw:
        float3 iHat_yaw = {cosf(yaw), 0, sinf(yaw)};
        float3 jHat_yaw = {0, 1, 0};
        float3 kHat_yaw = {-sinf(yaw), 0, cosf(yaw)};

        // Roll:
        float3 iHat_roll = {cosf(roll), sinf(roll), 0};
        float3 jHat_roll = {-sinf(roll), cosf(roll), 0};
        float3 kHat_roll = {0, 0, 1};

        // Pitch:
        float3 iHat_pitch = {1, 0, 0};
        float3 jHat_pitch = {0, cosf(pitch), -sinf(pitch)};
        float3 kHat_pitch = {0, sinf(pitch), cosf(pitch)};

        // Yaw and pitch combined:
        float3 iHat_pitchOnYaw = transformVector(iHat_yaw, jHat_yaw, kHat_yaw, iHat_pitch);
        float3 kHat_pitchonYaw = transformVector(iHat_yaw, jHat_yaw, kHat_yaw, kHat_pitch);
        float3 jHat_pitchOnYaw = transformVector(iHat_yaw, jHat_yaw, kHat_yaw, jHat_pitch);

        // All together:
        *iHat = transformVector(iHat_pitchOnYaw, jHat_pitchOnYaw, kHat_pitchonYaw, iHat_roll);
        *jHat = transformVector(iHat_pitchOnYaw, jHat_pitchOnYaw, kHat_pitchonYaw, jHat_roll);
        *kHat = transformVector(iHat_pitchOnYaw, jHat_pitchOnYaw, kHat_pitchonYaw, kHat_roll);
    }

    inline __attribute__((always_inline)) void Transform::computeInverseBasisVectors(float3* iHat, float3* jHat, float3* kHat) {
        // Yaw:
        float3 iHat_yaw = {cosf(-yaw), 0, sinf(-yaw)};
        float3 jHat_yaw = {0, 1, 0};
        float3 kHat_yaw = {-sinf(-yaw), 0, cosf(-yaw)};

        // Roll:
        float3 iHat_roll = {cosf(-roll), sinf(-roll), 0};
        float3 jHat_roll = {-sinf(-roll), cosf(-roll), 0};
        float3 kHat_roll = {0, 0, 1};

        // Pitch:
        float3 iHat_pitch = {1, 0, 0};
        float3 jHat_pitch = {0, cosf(-pitch), -sinf(-pitch)};
        float3 kHat_pitch = {0, sinf(-pitch), cosf(-pitch)};

        // Yaw and pitch combined:
        float3 iHat_yawOnPitch = transformVector(iHat_pitch, jHat_pitch, kHat_pitch, iHat_yaw);
        float3 jHat_yawOnPitch = transformVector(iHat_pitch, jHat_pitch, kHat_pitch, jHat_yaw);
        float3 kHat_yawOnPitch = transformVector(iHat_pitch, jHat_pitch, kHat_pitch, kHat_yaw);

        // All together:
        *iHat = transformVector(iHat_roll, jHat_roll, kHat_roll, iHat_yawOnPitch);
        *jHat = transformVector(iHat_roll, jHat_roll, kHat_roll, jHat_yawOnPitch);
        *kHat = transformVector(iHat_roll, jHat_roll, kHat_roll, kHat_yawOnPitch);
    }

    class Shader {
        public:
            // Sadly, my favourite words that you have seen throughout this header.hpp file on *literally* every single function will not apply
            // as the compiler can not inline this through a pointer to the base class

            // lightVector and normalVector must be unit vectors
            virtual uint32_t getColour(const float2&, const float3&, const float3&, const bool) const = 0;
            virtual ~Shader() = default;
    };

    class SolidColourShader : public Shader {
        public:
            uint32_t colour;
            SolidColourShader(uint32_t colour) : colour(colour) {};

            uint32_t getColour(const float2& UV, const float3& lightVector, const float3& normalVector, const bool useLight) const override {
                if (!useLight) return colour;

                float brightness = std::abs(float3::dot(lightVector, normalVector));
                if (brightness > 1) brightness = 1.0f;
                if (brightness < 0) brightness = 0.0f;

                brightness = brightness*0.5 + 0.5; // map it between [0.5,1]

                const uint8_t r = (uint8_t)(((colour&0xff0000)>>16) * brightness);
                const uint8_t g = (uint8_t)(((colour&0xff00)>>8) * brightness);
                const uint8_t b = (uint8_t)((colour&0xff) * brightness);

                const uint32_t col = (r << 16) | (g << 8) | b;

                return col;
            }
    };

    class TextureShader : public Shader {
        public:
            TextureShader(const std::vector<uint32_t>& pixels, const uint32_t w, const uint32_t h) : pixels(pixels), width(w), height(h) {};
            TextureShader(const TextureShader& other) : TextureShader(other.pixels, other.width, other.height) {}

            uint32_t getColour(const float2& UV, const float3& lightVector, const float3& normalVector, const bool useLight) const override {
                float2 uv = UV;

                if (uv.x > 1) uv.x = 1.0f;
                else if (uv.x < 0) uv.x = 0.0f;
                if (uv.y > 1) uv.y = 1.0f;
                else if (uv.y < 0) uv.y = 0.0f;

                // Nearest neighbour samplin'
                uint32_t x = (uint32_t)(uv.x*width);
                uint32_t y = (uint32_t)((1-uv.y)*height);
                if (x >= width) x = width-1;
                if (y >= height) y = height-1;
                if (x < 0) x = 0;
                if (y < 0) y = 0;


                if (!useLight) return pixels.at(y*width+x);

                const uint32_t col = pixels.at(y*width+x);

                float brightness = std::abs(float3::dot(lightVector, normalVector));
                if (brightness > 1) brightness = 1.0f;
                if (brightness < 0) brightness = 0.0f;

                brightness = brightness*0.5 + 0.5; // map it between [0.5,1]

                const uint8_t r = (uint8_t)(((col&0xff0000)>>16) * brightness);
                const uint8_t g = (uint8_t)(((col&0xff00)>>8) * brightness);
                const uint8_t b = (uint8_t)((col&0xff) * brightness);

                const uint32_t lightCol = (r << 16) | (g << 8) | b;

                return lightCol;
            }

            static inline __attribute__((always_inline)) TextureShader loadFromFile(const std::string& path) {
                // first uint32_t: width
                // second uint32_t: height
                // the rest: RGB
                std::ifstream f(path, std::ifstream::binary);
                if (!f.is_open()) {
                    throw std::runtime_error("Could not open file");
                }
                std::streampos begin = f.tellg(), end;
                if (begin == std::streampos(-1)) {
                    f.close();
                    throw std::runtime_error("tellg fail");
                }
                f.seekg(0, std::ifstream::end);
                end = f.tellg();
                if (end == std::streampos(-1)) {
                    f.close();
                    throw std::runtime_error("tellg fail");
                }
                f.seekg(0, std::ifstream::beg);
                auto size = end-begin;
                if (size < 8) {
                    f.close();
                    throw std::runtime_error("invalid size < 8");
                }

                std::vector<uint8_t> buffer(size);
                f.read(reinterpret_cast<char*>(buffer.data()), size);


                uint32_t width = 0, height = 0;
                // assume little-endian, this is our custom format after all and we set the standards
                width = buffer.at(0) | (buffer.at(1)<<8) | (buffer.at(2)<<16) | (buffer.at(3)<<24);
                height = buffer.at(4) | (buffer.at(5)<<8) | (buffer.at(6)<<16) | (buffer.at(7)<<24);
                if (buffer.size() < width*height*3+2*4 || width == 0 || height == 0) {
                    f.close();
                    throw std::runtime_error("Invalid image");
                }
                // f.read(&b, len);
                // f.gcount() return len of bytes actually read
                // f.eof() will be set
                // if len is more than what is left in the file, then it will read what's remaining
                // set EOF flag
                // gcount will return the # of bytes actually read
                f.close();

                std::vector<uint32_t> pixels(height*width, 0);
                buffer.erase(buffer.begin(), buffer.begin()+8); // remove the first 8 bytes, what remains will only be rgb values


                size_t index = 0;
                for (size_t i = 0; i < height; ++i) {
                    for (size_t j = 0; j < width; ++j) {
                        if (index+2>=buffer.size()) {
                            // this should never happen
                            throw std::runtime_error("Unexpected error happened, despite our already checking file sizes");
                        }
                        uint8_t r = buffer.at(index);
                        uint8_t g = buffer.at(index+1);
                        uint8_t b = buffer.at(index+2);
                        index += 3;
                        uint32_t col = ((uint32_t(r)&0xff)<<16) | ((uint32_t(g)&0xff)<<8) | (uint32_t(b)&0xff);
                        pixels.at(i*width+j) = col;
                    }
                }

                return TextureShader(pixels, width, height);
            }
        private:
            std::vector<uint32_t> pixels;
            uint32_t width, height;
    };

    struct Face {
        triangle3D verticies;
        std::optional<triangle> texCoords;
        std::optional<triangle3D> normals;
    };
    typedef struct Face Face;

    class Model {
        public:
            Model(Transform t) : transform(t) {};
            Transform transform;
            std::shared_ptr<Shader> shader = nullptr;
            std::vector<Face> faces;
            inline __attribute__((always_inline)) uint32_t getColour(const float2& uv, const float3& lightVector, const float3& normalVector, const bool useLight) {
                if (!shader) {
                    throw std::runtime_error("Null shader pointer");
                }
                return shader->getColour(uv, lightVector, normalVector, useLight);
            }
            static inline __attribute__((always_inline)) Model fromOBJ(const std::string&, const std::string&);
    };

    class Camera {
        public:
            Transform transform;
            float fov;
            float nearClip;
            Camera(Transform t) : transform(t), fov(60.0f), nearClip(0.001f) {};
    };

    class Scene {
        public:
            Scene(Camera cam) : camera(cam) {};
            std::vector<Model> models;
            Camera camera;
    };

    inline __attribute__((always_inline)) float signedTriangleArea(const float2& a, const float2& b, const float2& c) {
        const float2 ac = c - a;
        const float2 abPerp = float2::perpendicular(b - a);
        return float2::dot(ac, abPerp);
    }

    inline __attribute__((always_inline)) float absf(float a) {return a<0?-a:a;}

    inline __attribute__((always_inline)) bool pointInTriangle(const float2& a, const float2& b, const float2& c, const float2& p, float3* weights) {
        const float ABP = signedTriangleArea(a, b, p);
        const float BCP = signedTriangleArea(b, c, p);
        const float CAP = signedTriangleArea(c, a, p);
        const bool inTri = (ABP >= 0 && BCP >= 0 && CAP >= 0) || (ABP < 0 && BCP < 0 && CAP < 0);

        // Barycentric coordinates for weights of depth
        const float ABSABP = absf(ABP);
        const float ABSBCP = absf(BCP);
        const float ABSCAP = absf(CAP);
        const float invSum = 1/(ABSBCP + ABSCAP + ABSABP);
        weights->x = ABSBCP*invSum; // Weights A
        weights->y = ABSCAP*invSum; // Weights B
        weights->z = ABSABP*invSum; // Weights C
        // The depth will be the dot product of the zs and the weights,
        // [a.z,b.z,c.z]dot[weights.x,weights.y,weights.z]
        return inTri;
    }

    inline __attribute__((always_inline)) std::vector<triangle3D> triangulateQuad(float3 *faces, size_t size) {
        if (size != 4) throw std::runtime_error("size not 4");
        return {{faces[0], faces[1], faces[2]}, {faces[0],faces[2],faces[3]}};
    }


    inline __attribute__((always_inline)) std::optional<triangle3D> project(triangle3D tri, const Camera& camera, const int screenHeightPixels) {
        // p should be relative to the camera, and all transformations should already be applied
        if (tri.a.z <= camera.nearClip || tri.b.z <= camera.nearClip || tri.c.z <= camera.nearClip) {
            return std::nullopt;
        }
        const float screenHeight = 2*tanf(toRadians(camera.fov/2));
        const float pixelsPerWorldUnit = screenHeightPixels / screenHeight;

        tri.a.x = tri.a.x/tri.a.z*pixelsPerWorldUnit;
        tri.a.y = tri.a.y/tri.a.z*pixelsPerWorldUnit;
        tri.b.x = tri.b.x/tri.b.z*pixelsPerWorldUnit;
        tri.b.y = tri.b.y/tri.b.z*pixelsPerWorldUnit;
        tri.c.x = tri.c.x/tri.c.z*pixelsPerWorldUnit;
        tri.c.y = tri.c.y/tri.c.z*pixelsPerWorldUnit;
        return tri;
    }


    inline __attribute__((always_inline)) std::vector<std::string> split(std::string s, std::string delim) {
        std::vector<std::string> res;
        auto pos = s.find(delim);
        if (pos == std::string::npos) {
            res.push_back(s);
            return res;
        }
        while (pos != std::string::npos) {
            // after finding the first delim, take the substring
            res.push_back(s.substr(0, pos));

            s.erase(0, pos+1); // erase the substring from the string, including the delim

            pos = s.find(delim); // find the next delim
        }
        res.push_back(s);
        return res;
    }

    // Read from OBJ files
    // When exporting from blender, one must first mirror everything along y, then export with default settings (-z as forward and y as up)
    inline __attribute__((always_inline)) Model Model::fromOBJ(const std::string& objName, const std::string& texFile = "") {
        std::ifstream file(objName.c_str());
        if (!file.is_open()) throw std::runtime_error("Could not open file");

        std::optional<std::shared_ptr<TextureShader>> shader = std::nullopt;
        if (texFile != "") {
            shader = std::optional<std::shared_ptr<TextureShader>>(std::make_shared<TextureShader>(TextureShader::loadFromFile(texFile)));
        }

        try {
            std::vector<std::string> allLines;

            std::string tmp = "";
            while (std::getline(file, tmp)) {
                allLines.push_back(tmp);
            }
            file.close();

            tmp = "";
            std::vector<float3> verticies;
            std::vector<float2> texCoords;
            std::vector<float3> normals;

            std::vector<Face> faces;

            for (int i = 0; i < allLines.size(); ++i) {
                std::string line = allLines[i];
                if (line.substr(0,2) == "v ") {
                    std::vector<std::string> values;
                    values = split(line, " ");

                    // we expect points to be a 4, or 5 dimensional vector (since the leading v is included), all containing floats
                    if (values.size() != 4 && values.size() != 5) throw std::exception();

                    const float w = (values.size() == 5) ? std::stof(values[4]) : 1;
                    float3 v;
                    v.x = std::stof(values[1]) / w;
                    v.y = std::stof(values[2]) / w;
                    v.z = std::stof(values[3]) / w;

                    // float3{ x=-1, y=-1, z=1 }
                    // std::cout << "float3{ x=" << v.x << ", y=" << v.y << ", z=" << v.z << " }" << std::endl;

                    verticies.push_back(v);
                } else if (line.substr(0,2) == "vt") {
                    std::vector<std::string> values;
                    values = split(line, " ");
                    if (values.size() != 3 && values.size() != 4) throw std::exception();
                    float2 v;
                    // For UV maps, U corrosponds to the x coordinate on the image
                    v.x = std::stof(values[1]);
                    v.y = std::stof(values[2]);
                    texCoords.push_back(v);
                } else if (line.substr(0,2) == "vn") {
                    std::vector<std::string> values;
                    values = split(line, " ");
                    if (values.size() != 4) throw std::exception();

                    float3 v;
                    v.x = std::stof(values[1]);
                    v.y = std::stof(values[2]);
                    v.z = std::stof(values[3]);
                    normals.push_back(v.normalize()); // make it a unit vector, since it may not be a unit vector in the obj file
                }
            }

            for (int i = 0; i < allLines.size(); ++i) {
                std::string line = allLines[i];
                if (line.substr(0, 2) == "f ") {
                    std::vector<std::string> spaceSeperated;
                    spaceSeperated = split(line, " ");

                    // the overal 2d vector describing all the things, this should be a 3x3 matrix with
                    // each row being an entry in spaceSeperated
                    std::vector<std::vector<std::string>> slashSeperated;

                    // start at 1 due to the "f "
                    for (int j = 1; j < spaceSeperated.size(); ++j) {
                        slashSeperated.push_back(split(spaceSeperated.at(j), "/"));
                    }

                    if (slashSeperated.size() != 3) {
                        throw std::exception();
                    }

                    uint64_t indexA = std::stoull(slashSeperated.at(0).at(0)) - 1;
                    uint64_t indexB = std::stoull(slashSeperated.at(1).at(0)) - 1;
                    uint64_t indexC = std::stoull(slashSeperated.at(2).at(0)) - 1;

                    Face face;
                    face.verticies = {verticies.at(indexA), verticies.at(indexB), verticies.at(indexC)};

                    indexA = std::stoull(slashSeperated.at(0).at(1)) - 1;
                    indexB = std::stoull(slashSeperated.at(1).at(1)) - 1;
                    indexC = std::stoull(slashSeperated.at(2).at(1)) - 1;

                    face.texCoords = {texCoords.at(indexA), texCoords.at(indexB), texCoords.at(indexC)};

                    indexA = std::stoull(slashSeperated.at(0).at(2)) - 1;
                    indexB = std::stoull(slashSeperated.at(1).at(2)) - 1;
                    indexC = std::stoull(slashSeperated.at(2).at(2)) - 1;

                    face.normals = {normals.at(indexA), normals.at(indexB), normals.at(indexC)};

                    faces.push_back(face);
                }
            }

            // create a vector of triangle3D now to go with our model
            Model model(Transform({0,0,0},0,0,0)); // use a default transform, the caller can change this later

            model.faces = faces;

            if (shader.has_value()) {
                model.shader = shader.value();
            }

            return model;

        } catch (std::exception) {
            throw std::runtime_error("Bad OBJ File");
        }
    }
}
