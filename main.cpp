#include <SDL2/SDL.h>
#include "raster.hpp"
#include <SDL2/SDL_scancode.h>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>

using namespace std;
using namespace Raster;

#define SPEED 9.8f
#define ROTATION_SPEED 45.0f


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Blue: 0xff87ceeb
#define BACKGROUND 0xff87ceeb

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "no" << endl;
        return EXIT_FAILURE;
    }
    const filesystem::path exePath(getExecutablePath());
    const filesystem::path exeDir(exePath.string().substr(0,exePath.string().length()-exePath.filename().string().length()));


    SDL_Window* window = SDL_CreateWindow("Rasterizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_VULKAN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    Scene scene(Camera(Transform({0, 0, 0}, 0, 0, 0)));

    // create a triangle
    shared_ptr<Model> spinTri = make_shared<Model>(make_shared<Transform>(Transform({0, 1, 0}, 0, 0, 0)));
    spinTri->faces.push_back({{{-1, -1, 0}, {0, 1, 0}, {1, -1, 0}}, std::nullopt, std::nullopt});
    spinTri->shader = std::make_shared<SolidColourShader>(0xe6a000);

    shared_ptr<Model> spinTriT = make_shared<Model>(make_shared<Transform>(Transform({0, 1, 0}, 90, 0, 0)));
    spinTriT->faces.push_back({{{-1, -1, 0}, {0, 1, 0}, {1, -1, 0}}, std::nullopt, std::nullopt});
    spinTriT->shader = std::make_shared<SolidColourShader>(0xe6a0ff);

    shared_ptr<Model> pinkTri = make_shared<Model>(make_shared<Transform>(Transform({5.0f, 1, 5.0f}, 0, 0, 0)));
    pinkTri->faces.push_back({{{-1, -1, 0}, {0, 1, 0}, {1, -1, 0}}, std::nullopt, std::nullopt});
    pinkTri->shader = std::make_shared<SolidColourShader>(0xe6a0ff);

    shared_ptr<Model> whiteTri = make_shared<Model>(make_shared<Transform>(Transform({-5.0f, 1, -5.0f}, 0, 0, 0)));
    whiteTri->faces.push_back({{{-1, -1, 0}, {0, 1, 0}, {1, -1, 0}}, std::nullopt, std::nullopt});
    whiteTri->shader = std::make_shared<SolidColourShader>(0xffffff);

    shared_ptr<Model> greenTri = make_shared<Model>(make_shared<Transform>(Transform({0, 5, 0}, 0, 90, 0)));
    greenTri->faces.push_back({{{-1, -1, 0},{0, 1, 0},{1, -1, 0}}, std::nullopt, std::nullopt});
    greenTri->shader = std::make_shared<SolidColourShader>(0x00aa00);

    float cubeSize = 10;
    shared_ptr<Model> ground = make_shared<Model>(make_shared<Transform>(Transform({0, -cubeSize, 0}, 0, 0, 0)));
    ground->faces.push_back({{{-cubeSize, cubeSize, -cubeSize}, {cubeSize, cubeSize, -cubeSize}, {-cubeSize, cubeSize, cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, cubeSize, cubeSize}, {cubeSize, cubeSize, cubeSize}, {cubeSize, cubeSize, -cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, cubeSize, -cubeSize}, {-cubeSize, -cubeSize, -cubeSize}, {cubeSize, -cubeSize, -cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, cubeSize, -cubeSize}, {cubeSize, cubeSize, -cubeSize}, {cubeSize, -cubeSize, -cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, cubeSize, cubeSize}, {-cubeSize, cubeSize, -cubeSize}, {-cubeSize, -cubeSize, cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, cubeSize, -cubeSize}, {-cubeSize, -cubeSize, -cubeSize}, {-cubeSize, -cubeSize, cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, cubeSize, cubeSize}, {cubeSize, cubeSize, cubeSize}, {cubeSize, -cubeSize, cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, cubeSize, cubeSize}, {-cubeSize, -cubeSize, cubeSize}, {cubeSize, -cubeSize, cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{cubeSize, cubeSize, cubeSize}, {cubeSize, cubeSize, -cubeSize}, {cubeSize, -cubeSize, cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{cubeSize, cubeSize, -cubeSize}, {cubeSize, -cubeSize, -cubeSize}, {cubeSize, -cubeSize, cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, -cubeSize, -cubeSize}, {cubeSize, -cubeSize, -cubeSize}, {-cubeSize, -cubeSize, cubeSize}}, std::nullopt, std::nullopt});
    ground->faces.push_back({{{-cubeSize, -cubeSize, cubeSize}, {cubeSize, -cubeSize, cubeSize}, {cubeSize, -cubeSize, -cubeSize}}, std::nullopt, std::nullopt});
    ground->shader = std::make_shared<SolidColourShader>(0xdddddd);


    shared_ptr<Model> monkey = Model::fromOBJ(exeDir.string() + "resources/monkey.obj", exeDir.string() + "resources/rock.bin");
    monkey->transform->position = {0, 2, 10};
    monkey->transform->setRotation(180, 0, 0);
    monkey->shader = std::make_shared<SolidColourShader>(0xdddddd);

    shared_ptr<Model> objCube = Model::fromOBJ(exeDir.string() + "resources/Nefertitiobj.obj");
    objCube->transform->position = {-2, 3, -10};
    objCube->shader = std::make_shared<SolidColourShader>(0xffffff);

    shared_ptr<Model> axes = Model::fromOBJ(exeDir.string() + "resources/axis.obj", exeDir.string() + "resources/RGB.bin");
    axes->transform->position = {2, 2, 2};

    shared_ptr<Model> axes2 = Model::fromOBJ(exeDir.string() + "resources/axis.obj", exeDir.string() + "resources/RGB.bin");
    axes2->transform->position = {0,0,3};
    axes2->transform->parent = axes->transform;

    shared_ptr<Model> sphere = Model::fromOBJ(exeDir.string() + "resources/cube.obj");
    sphere->transform->position = {0,0,3};
    sphere->shader = make_shared<SolidColourShader>(0xdddddd);
    sphere->transform->parent = axes2->transform;

    // Model minecraft = Model::fromOBJ(exeDir.string() + "resources/minecraftCube.obj", exeDir.string() + "resources/grass.bin");
    // minecraft.transform.position = {-2, 2, 2};

    scene.models.push_back(spinTri); // 0
    scene.models.push_back(pinkTri); // 1
    scene.models.push_back(whiteTri);// 2
    scene.models.push_back(greenTri);// 3
    scene.models.push_back(ground);  // 4
    scene.models.push_back(spinTriT);// 5
    scene.models.push_back(monkey);  // 6
    scene.models.push_back(objCube); // 7
    scene.models.push_back(axes);    // 8
    scene.models.push_back(axes2);
    scene.models.push_back(sphere);
    // scene.models.push_back(minecraft);

    bool run = true;
    uint8_t wasdqe = 0b000000;
    uint8_t lrud = 0b0000;

    auto newTime = chrono::high_resolution_clock::now();
    chrono::time_point<chrono::high_resolution_clock> oldTime = newTime;

    // Animation variables
    bool greenTriMoveUp = true;
    int pinkTriState = 0;
    auto pinkTriEnd = newTime;

    float (*depthBuffer)[SCREEN_WIDTH] = (float(*)[SCREEN_WIDTH])std::malloc(SCREEN_HEIGHT*SCREEN_WIDTH*sizeof(float));
    if (depthBuffer == NULL) {
        cerr << "malloc fail" << endl;
        return EXIT_FAILURE;
    }

    // This is the PROPER spelling of the word "colour", if you disagree please consult https://en.wiktionary.org/wiki/colour
    uint32_t (*colourBuffer)[SCREEN_WIDTH] = (uint32_t(*)[SCREEN_WIDTH])std::malloc(SCREEN_HEIGHT*SCREEN_WIDTH*sizeof(uint32_t)); // colourBuffer is ARGB
    if (colourBuffer == NULL) {
        cerr << "malloc fail" << endl;
        return EXIT_FAILURE;
    }


    bool doAnimations = true;
    while (run) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                run = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.scancode) {
                    // Movement
                    case SDL_SCANCODE_W: wasdqe |= (1<<5); break;
                    case SDL_SCANCODE_A: wasdqe |= (1<<4); break;
                    case SDL_SCANCODE_S: wasdqe |= (1<<3); break;
                    case SDL_SCANCODE_D: wasdqe |= (1<<2); break;
                    case SDL_SCANCODE_Q: wasdqe |= (1<<1); break;
                    case SDL_SCANCODE_E: wasdqe |= (1<<0); break;
                    // Rotations
                    case SDL_SCANCODE_LEFT: lrud |= (1<<3); break;
                    case SDL_SCANCODE_RIGHT: lrud |= (1<<2); break;
                    case SDL_SCANCODE_UP: lrud |= (1<<1); break;
                    case SDL_SCANCODE_DOWN: lrud |= (1<<0); break;
                    // Toggle the animations
                    case SDL_SCANCODE_SPACE: doAnimations = !doAnimations; break;
                    default: break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.scancode) {
                    // Movement
                    case SDL_SCANCODE_W: wasdqe &= ~(1<<5); break;
                    case SDL_SCANCODE_A: wasdqe &= ~(1<<4); break;
                    case SDL_SCANCODE_S: wasdqe &= ~(1<<3); break;
                    case SDL_SCANCODE_D: wasdqe &= ~(1<<2); break;
                    case SDL_SCANCODE_Q: wasdqe &= ~(1<<1); break;
                    case SDL_SCANCODE_E: wasdqe &= ~(1<<0); break;
                    // Rotations
                    case SDL_SCANCODE_LEFT: lrud &= ~(1<<3); break;
                    case SDL_SCANCODE_RIGHT: lrud &= ~(1<<2); break;
                    case SDL_SCANCODE_UP: lrud &= ~(1<<1); break;
                    case SDL_SCANCODE_DOWN: lrud &= ~(1<<0); break;
                    default: break;
                }
            }
        }
        if (!run) break;

        newTime = chrono::high_resolution_clock::now();
        float deltaTime = chrono::duration<float>(newTime-oldTime).count();
        oldTime = newTime;

        if (doAnimations) {
            scene.models[0]->transform->incYaw(270.0f*deltaTime);
            scene.models[4]->transform->incYaw(45.0f*deltaTime);
            scene.models[5]->transform->incYaw(180.0f*deltaTime);
            scene.models[3]->transform->incRoll(30.0f*deltaTime);
            scene.models[3]->transform->incPitch(30.0f*deltaTime);
            scene.models[8]->transform->incYaw(60.0f*deltaTime);
            scene.models[8]->transform->incPitch(60.0f*deltaTime);
            scene.models[8]->transform->incRoll(60.0f*deltaTime);
            scene.models[9]->transform->incYaw(60.0f*deltaTime);
            scene.models[6]->transform->incYaw(60.0f*deltaTime);
            scene.models[9]->transform->incPitch(60.0f*deltaTime);
            scene.models[9]->transform->incRoll(60.0f*deltaTime);

            if (scene.models[3]->transform->position.y > 15) {
                greenTriMoveUp = false;
            } else if (scene.models[3]->transform->position.y < 5) {
                greenTriMoveUp = true;
            }

            scene.models[3]->transform->position.y += (greenTriMoveUp ? 1 : -1)*deltaTime*5;

            if (pinkTriState == 0) {
                scene.models[1]->transform->incYaw(15.0f*deltaTime);
                if (scene.models[1]->transform->getYaw() >= 135.0f) {
                    scene.models[1]->transform->setYaw(135.0f);
                    ++pinkTriState;
                }
            } else if (pinkTriState == 1) {
                scene.models[1]->transform->incPitch(15.0f*deltaTime);
                if (scene.models[1]->transform->getPitch() >= 70.0f) {
                    scene.models[1]->transform->setPitch(70.0f);
                    ++pinkTriState;
                }
            } else if (pinkTriState == 2) {
                scene.models[1]->transform->incRoll(15.0f*deltaTime);
                if (scene.models[1]->transform->getRoll() >= 90.0f) {
                    scene.models[1]->transform->setRoll(90.0f);
                    ++pinkTriState;
                    pinkTriEnd = chrono::high_resolution_clock::now();
                }
            } else if (pinkTriState == 3 && chrono::duration<float>(newTime-pinkTriEnd).count() >= 5.0f) {
                scene.models[1]->transform->setRotation(0, 0, 0);
                pinkTriState = 0;
            }
        }


        if (lrud & (1<<3)) scene.camera.transform.setYaw(scene.camera.transform.getYaw() + deltaTime*ROTATION_SPEED);
        if (lrud & (1<<2)) scene.camera.transform.setYaw(scene.camera.transform.getYaw() - deltaTime*ROTATION_SPEED);
        if (lrud & (1<<1)) scene.camera.transform.setPitch(scene.camera.transform.getPitch() + deltaTime*ROTATION_SPEED);
        if (lrud & (1<<0)) scene.camera.transform.setPitch(scene.camera.transform.getPitch() - deltaTime*ROTATION_SPEED);
        float3 ihatcam, jhatcam, khatcam;
        float3 ihatcaminv, jhatcaminv, khatcaminv;
        scene.camera.transform.fetchBasisVectors(&ihatcam, &jhatcam, &khatcam);
        scene.camera.transform.fetchInverseBasisVectors(&ihatcaminv, &jhatcaminv, &khatcaminv);
        // cout << khatcam.x << " " << khatcam.y << " " << khatcam.z << endl;
        if (wasdqe & (1<<5)) scene.camera.transform.position += deltaTime*SPEED*Transform::transformVector(ihatcam, jhatcam, khatcam, float3_forward);
        if (wasdqe & (1<<4)) scene.camera.transform.position -= deltaTime*SPEED*Transform::transformVector(ihatcam, jhatcam, khatcam, float3_right);
        if (wasdqe & (1<<3)) scene.camera.transform.position -= deltaTime*SPEED*Transform::transformVector(ihatcam, jhatcam, khatcam, float3_forward);
        if (wasdqe & (1<<2)) scene.camera.transform.position += deltaTime*SPEED*Transform::transformVector(ihatcam, jhatcam, khatcam, float3_right);
        if (wasdqe & (1<<1)) scene.camera.transform.position -= deltaTime*SPEED*Transform::transformVector(ihatcam, jhatcam, khatcam, float3_up);
        if (wasdqe & (1<<0)) scene.camera.transform.position += deltaTime*SPEED*Transform::transformVector(ihatcam, jhatcam, khatcam, float3_up);

        for (int i = 0; i < SCREEN_HEIGHT; ++i) {
            for (int j = 0; j < SCREEN_WIDTH; ++j) {
                depthBuffer[i][j] = INFINITY;
                colourBuffer[i][j] = BACKGROUND;
            }
        }

        for (int i = 0; i < scene.models.size(); ++i) {
            const auto model = scene.models[i];
            float3 ihat, jhat, khat;
            model->transform->fetchBasisVectorsRecursive(&ihat, &jhat, &khat);


            for (int j = 0; j < model->faces.size(); ++j) {
                const Face& face = model->faces[j];
                // compute the new verticies of face based on the transformations in model.transform
                // make point relative to camera, then rotate according to camera rotations (do later)
                // compute the bounding box of each triangle, and test if a pixel is in the triangle
                // if it is in the triangle, draw the pixel
                // do some stuff with a depth buffer

                float3 a = face.verticies.a, b = face.verticies.b, c = face.verticies.c;

                // Rotate along the model's basis vectors
                a = Transform::transformVector(ihat, jhat, khat, a);
                b = Transform::transformVector(ihat, jhat, khat, b);
                c = Transform::transformVector(ihat, jhat, khat, c);

                const float3 pos = model->transform->getAbsolutePosition();
                // Add the model's position
                a += pos;
                b += pos;
                c += pos;

                // Now make it relative to the camera
                a -= scene.camera.transform.position;
                b -= scene.camera.transform.position;
                c -= scene.camera.transform.position;

                // Rotate along the inverse basis vectors of the camera so that it looks like we are rotating the cameras
                a = Transform::transformVector(ihatcaminv, jhatcaminv, khatcaminv, a);
                b = Transform::transformVector(ihatcaminv, jhatcaminv, khatcaminv, b);
                c = Transform::transformVector(ihatcaminv, jhatcaminv, khatcaminv, c);


                float3 nA = face.normals.value_or({a, float3_zero, float3_zero}).a;
                float3 nB = face.normals.value_or({a, float3_zero, float3_zero}).b;
                float3 nC = face.normals.value_or({a, float3_zero, float3_zero}).c;

                // Rotate along the model's transform
                nA = Transform::transformVector(ihat, jhat, khat, nA);
                nB = Transform::transformVector(ihat, jhat, khat, nB);
                nC = Transform::transformVector(ihat, jhat, khat, nC);

                // Rotate along the inverse of the camera
                nA = Transform::transformVector(ihatcaminv, jhatcaminv, khatcaminv, nA);
                nB = Transform::transformVector(ihatcaminv, jhatcaminv, khatcaminv, nB);
                nC = Transform::transformVector(ihatcaminv, jhatcaminv, khatcaminv, nC);


                auto projectedTriangle = project({a,b,c}, scene.camera, SCREEN_HEIGHT);
                if (projectedTriangle.has_value()) {
                    const triangle3D& tri = projectedTriangle.value();
                    float minX = max(min(tri.a.x, min(tri.b.x, tri.c.x)),-SCREEN_WIDTH/2.0f)+SCREEN_WIDTH/2.0f;
                    float minY = SCREEN_HEIGHT/2.0f-max(min(tri.a.y, min(tri.b.y, tri.c.y)), -SCREEN_HEIGHT/2.0f);
                    float maxX = min(max(tri.a.x, max(tri.b.x, tri.c.x)),SCREEN_WIDTH/2.0f)+SCREEN_WIDTH/2.0f;
                    float maxY = SCREEN_HEIGHT/2.0f-min(max(tri.a.y, max(tri.b.y, tri.c.y)),SCREEN_HEIGHT/2.0f);

                    // Comment out to hide the bounding box
                    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    // SDL_RenderDrawLine(renderer, minX, minY, minX, maxY);
                    // SDL_RenderDrawLine(renderer, minX, maxY, maxX, maxY);
                    // SDL_RenderDrawLine(renderer, maxX, maxY, maxX, minY);
                    // SDL_RenderDrawLine(renderer, maxX, minY, minX, minY);


                    // Clamp the bounds
                    if (minY >= SCREEN_HEIGHT) {
                        minY = SCREEN_HEIGHT-1;
                    } else if (minY < 0) {
                        minY = 0;
                    }
                    if (maxY >= SCREEN_HEIGHT) {
                        maxY = SCREEN_HEIGHT-1;
                    } else if (maxY < 0) {
                        maxY = 0;
                    }

                    if (minX >= SCREEN_WIDTH) {
                        minX = SCREEN_WIDTH-1;
                    } else if (minX < 0) {
                        minX = 0;
                    }
                    if (maxX >= SCREEN_WIDTH) {
                        maxX = SCREEN_WIDTH-1;
                    } else if (maxX < 0) {
                        maxX = 0;
                    }

                    for (int k = minY; k >= maxY && k >= 0 && k < SCREEN_HEIGHT; --k) {
                        for (int l = minX; l <= maxX && l >= 0 && l < SCREEN_WIDTH; ++l) {
                            float2 p;
                            p.x = l-SCREEN_WIDTH/2.0f;
                            p.y = SCREEN_HEIGHT/2.0f-k;
                            float3 weight;
                            if (pointInTriangle(tri.a.dropZ(), tri.b.dropZ(), tri.c.dropZ(), p, &weight)) {
                                float3 _depths = {tri.a.z, tri.b.z, tri.c.z};
                                float depth = 1/float3::dot(1/_depths, weight);

                                if (depth > depthBuffer[k][l]) continue;
                                depthBuffer[k][l] = depth;


                                float2 uv = {0,0};
                                if (face.texCoords != nullopt) {
                                    const triangle& uvs = face.texCoords.value();
                                    const float3 u = {uvs.a.x/tri.a.z, uvs.b.x/tri.b.z, uvs.c.x/tri.c.z};
                                    const float3 v = {uvs.a.y/tri.a.z, uvs.b.y/tri.b.z, uvs.c.y/tri.c.z};

                                    uv.x = float3::dot(u,weight)*depth;
                                    uv.y = float3::dot(v,weight)*depth;
                                }

                                // To undo the perspective, we can divide by pixelsPerWorldUnit, and then multiply by the interpolated depth
                                const float screenHeightUnits = 2*tanf(toRadians(scene.camera.fov/2));
                                const float pixelsPerWorldUnit = SCREEN_HEIGHT / screenHeightUnits;

                                float3 actualPoint{p.x/pixelsPerWorldUnit*depth, p.y/pixelsPerWorldUnit*depth, depth};
                                actualPoint = actualPoint.normalize();

                                uint32_t col = model->getColour(uv, actualPoint, nA.normalize(), face.normals.has_value()) | 0xff000000; // add 0xff000000 for the alpha, this must be opaqe

                                colourBuffer[k][l] = col;
                            }
                        }
                    }
                }
            }
        }

        // use an SDL_Texture to write to it
        SDL_UpdateTexture(texture, NULL, colourBuffer, SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // cout << 1/deltaTime << endl;
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_Quit();

    free(colourBuffer);
    free(depthBuffer);
    return EXIT_SUCCESS;
}
