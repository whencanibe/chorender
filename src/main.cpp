#include <GL/glew.h>

#include <iostream>
#include <exception>

#include "platform/window.h"
#include "platform/input.h"
#include "platform/imgui_backend.h"

#include <imgui.h>

#include "core/shader.h"
#include "core/model.h"
#include "core/camera.h"
#include "core/framebuffer.h"
#include "helper/matrix.h"
#include "shapes/cube.h"
#include "renderer/skybox.h"
#include "renderer/screen_quad.h"
#include "renderer/light.h"
#include "renderer/shadow_map.h"

constexpr int kWindowWidth = 1200;
constexpr int kWindowHeight = 800;

int main()
try
{
    Window window(kWindowWidth, kWindowHeight, "Sponza");
    Input input(window);
    ImGuiBackend imgui(window);
    Camera camera(Vec3(0.0f, 0.0f, 3.0f));

    Shader shader("../src/shaders/basic.vert", "../src/shaders/basic.frag");
    Cube cube;

    Shader lightShader("../src/shaders/lightShader.vert", "../src/shaders/lightShader.frag");

    // Directional shadow map. Unit 8 keeps clear of the material slots Mesh::Draw uses (0-3).
    constexpr int kShadowMapUnit = 8;
    ShadowMap shadowMap(2048);
    lightShader.use();
    lightShader.setInt("shadowMap", kShadowMapUnit);
    bool shadowsEnabled = true;
    float shadowBias = 0.005f;
    int pcfRadius = 1;

    // Lights. Point lights are spread along the atrium on X, raised above the floor.
    DirLight sun;
    sun.direction = Vec3(1.0f, -1.0f, 1.0f);

    PointLight pointLights[4];
    pointLights[0].position = Vec3(-6.0f, 5.0f, 0.0f);
    pointLights[1].position = Vec3(-2.0f, 5.0f, 0.0f);
    pointLights[2].position = Vec3(2.0f, 5.0f, 0.0f);
    pointLights[3].position = Vec3(6.0f, 5.0f, 0.0f);

    SpotLight flashlight; // follows the camera; position/direction refreshed every frame

    // Point lights are static: uniforms persist per program, so upload once.
    // The sun is edited from the UI, so it is re-uploaded every frame.
    lightShader.use();
    for (int i = 0; i < 4; i++)
        UploadLight(lightShader, pointLights[i], i);

    Model sponza("../assets/crytek_sponza/sponza.obj");
    Shader screenShader("../src/shaders/screen.vert", "../src/shaders/screen.frag");

    // Scene renders linear HDR into this FBO; the post pass (screenShader) tone maps + gamma encodes it.
    // RGBA16F keeps values above 1.0 and avoids banding in the darks that 8-bit linear would show.
    // Sized from the real framebuffer, not the window, so it stays correct if Retina scaling is enabled.
    int fbW, fbH;
    window.GetFramebufferSize(fbW, fbH);
    FramebufferSpec sceneSpec;
    sceneSpec.width = fbW;
    sceneSpec.height = fbH;
    sceneSpec.colorInternalFormat = GL_RGBA16F;
    Framebuffer sceneFbo(sceneSpec);
    ScreenQuad screenQuad;
    float exposure = 1.0f;

    Skybox skybox({
        "../assets/skybox/right.jpg",
        "../assets/skybox/left.jpg",
        "../assets/skybox/top.jpg",
        "../assets/skybox/bottom.jpg",
        "../assets/skybox/front.jpg",
        "../assets/skybox/back.jpg",
    });

    bool spotLightOn = true;

    // Sponza is authored in centimeters; 0.01 brings it to meters.
    Mat4 sponzaModel = Scale(Mat4(), Vec3(0.01f));
    Mat3 sponzaNormalMat = inverseTranspose(Mat3(sponzaModel));
    // World-space bounding sphere the shadow map must cover (crytek bounds x 0.01).
    const Vec3 sceneCenter(-0.6f, 6.5f, -0.4f);
    const float sceneRadius = 24.0f;

    // Model loading stalled the loop; drop the cursor jump and restart the frame clock.
    window.PollEvents();
    input.ResetMouse();
    float lastFrame = static_cast<float>(window.GetTime());

    while (!window.ShouldClose())
    {
        window.PollEvents();

        // Re-read every frame: the backing store can change size without a resize event
        // (e.g. the window moving to a display with a different scale).
        window.GetFramebufferSize(fbW, fbH);
        sceneFbo.Resize(fbW, fbH); // no-op when unchanged

        float currentFrame = static_cast<float>(window.GetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        input.Update(camera, deltaTime);
        if (input.WasKeyPressed(GLFW_KEY_TAB))
            input.SetCameraMode(!input.IsCameraMode());
        if (input.WasKeyPressed(GLFW_KEY_Q))
            spotLightOn = !spotLightOn;

        // UI is declared here so its values are applied in this same frame; drawn after the post pass.
        imgui.BeginFrame();
        ImGui::Begin("Renderer");
        ImGui::Text("%.1f fps (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::TextDisabled("Tab: toggle camera / UI   Q: flashlight");
        ImGui::Separator();
        ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
        ImGui::Checkbox("Flashlight", &spotLightOn);
        if (ImGui::CollapsingHeader("Sun", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat3("Direction", &sun.direction.X, -1.0f, 1.0f);
            ImGui::ColorEdit3("Diffuse", &sun.diffuse.X);
            ImGui::ColorEdit3("Ambient", &sun.ambient.X);
        }
        if (ImGui::CollapsingHeader("Shadows", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Checkbox("Enabled", &shadowsEnabled);
            ImGui::SliderFloat("Bias", &shadowBias, 0.0f, 0.02f, "%.4f");
            ImGui::SliderInt("PCF radius", &pcfRadius, 0, 3);
            // GL textures are bottom-up; flip V so the map is not upside down.
            ImGui::Image((ImTextureID)(intptr_t)shadowMap.DepthTexture(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        // shadow pass: scene depth from the sun
        const Mat4 lightSpace = ShadowMap::ComputeLightSpace(sun.direction, sceneCenter, sceneRadius);
        shadowMap.Begin(lightSpace);
        shadowMap.GetShader().setMat4("uModel", sponzaModel);
        sponza.Draw(shadowMap.GetShader());
        shadowMap.End();

        // scene pass: scene -> sceneFbo
        sceneFbo.Bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // sceneFbo matches the framebuffer size (reconciled at the top of the loop)
        const float aspect = static_cast<float>(sceneFbo.Width()) / static_cast<float>(sceneFbo.Height());

        shader.use();
        Mat4 view = camera.GetViewMatrix();
        Mat4 proj = Perspective(toRadian(camera.Zoom), aspect, 0.1f, 100.0f);

        shader.setMat4("uView", view);
        shader.setMat4("uProj", proj);
        for (uint32_t i = 0; i < 4; i++)
        {
            Mat4 model = Mat4();
            model = Translate(model, pointLights[i].position);
            model = Scale(model, Vec3(0.2f));

            shader.setMat4("uModel", model);
            cube.Draw(shader);
        }

        lightShader.use();
        lightShader.setVec3("uViewPos", camera.Position);
        lightShader.setMat4("uProj", proj);
        lightShader.setMat4("uView", view);

        lightShader.setMat4("uModel", sponzaModel);
        lightShader.setMat3("uNormalMat", sponzaNormalMat);

        lightShader.setMat4("uLightSpace", lightSpace);
        lightShader.setBool("uShadowsEnabled", shadowsEnabled);
        lightShader.setFloat("uShadowBias", shadowBias);
        lightShader.setInt("uPcfRadius", pcfRadius);
        glActiveTexture(GL_TEXTURE0 + kShadowMapUnit);
        glBindTexture(GL_TEXTURE_2D, shadowMap.DepthTexture());

        UploadLight(lightShader, sun);

        flashlight.position = camera.Position;
        flashlight.direction = camera.Front;
        UploadLight(lightShader, flashlight);

        lightShader.setBool("ubIsSpotLight", spotLightOn);

        sponza.Draw(lightShader);

        skybox.Draw(view, proj); // after all opaque geometry

        // post pass: sceneFbo -> screen
        Framebuffer::Unbind();
        glViewport(0, 0, fbW, fbH); // Unbind() does not touch the viewport; the default framebuffer needs its own
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        screenShader.setFloat("uExposure", exposure);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneFbo.ColorTexture());
        screenQuad.Draw();

        imgui.EndFrame();

        window.SwapBuffers();
    }

    glDeleteProgram(shader.ID);
    return 0;
}
catch (const std::exception &e)
{
    std::cerr << e.what() << std::endl;
    return -1;
}
