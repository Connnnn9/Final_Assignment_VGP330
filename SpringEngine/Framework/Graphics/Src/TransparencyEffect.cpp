#include "Precompiled.h"
#include "TransparencyEffect.h"
#include "GraphicsSystem.h"
#include "TextureManager.h"
#include "VertexTypes.h"
#include "Camera.h"
#include "RenderObject.h"

using namespace SpringEngine;
using namespace SpringEngine::Graphics;

void TransparencyEffect::Initialize(const std::filesystem::path& filePath)
{
    mTransformBuffer.Initialize();

    mVertexShader.Initialize<Vertex>(filePath);
    mPixelShader.Initialize(filePath);

    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void TransparencyEffect::Terminate()
{
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();
    mTransformBuffer.Terminate();
}

void TransparencyEffect::Begin()
{
    mVertexShader.Bind();
    mPixelShader.Bind();
    mSampler.BindPS(0);
}

void TransparencyEffect::End()
{
}

void TransparencyEffect::Render(const RenderObject& renderObject)
{
    const auto& matWorld = renderObject.transform.GetMatrix4();
    const auto& matView = mCamera->GetViewMatrix();
    const auto& matProj = mCamera->GetProjectionMatrix();

    TransformData data;
    data.wvp = Transpose(matWorld * matView * matProj);
    data.world = Transpose(matWorld);
   
    data.ViewPosition = mCamera->GetPosition();

    mTransformBuffer.Update(data);

    mTransformBuffer.BindVS(0);

    auto tm = TextureManager::Get();
    if (renderObject.diffuseMapId > 0) tm->BindPS(renderObject.diffuseMapId, 0);

    renderObject.meshBuffer.Render();
}

void TransparencyEffect::SetCamera(const Camera& camera)
{
    mCamera = &camera;
}

void TransparencyEffect::SetTransparencyValue(float value)
{
   
    SettingsData settingsData;
    settingsData.transparency = value;
    mSettingsBuffer.Update(settingsData);
    mSettingsBuffer.BindPS(0); 
}

void TransparencyEffect::DebugUI()
{
    if (ImGui::CollapsingHeader("TransparencyEffect", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static float transparency = 1.0f;
        if (ImGui::SliderFloat("Transparency", &transparency, 0.0f, 1.0f))
        {
            SetTransparencyValue(transparency);
        }
    }
}
