#pragma once

#include"ConstantBuffer.h"
#include"PixelShader.h"
#include"Sampler.h"
#include"RenderTarget.h"
#include"VertexShader.h"

namespace SpringEngine::Graphics
{
	class RenderObject;

	class GaussianBlurEffect final
	{
	public:
		void Initialize();
		void Terminate();

		void Begin();
		void End();

		void Render(const RenderObject& renderObject);

		void DebugUI();

		void SetSourceTexture(const Texture& texture);
		const Texture& GetHorizontalBlurTexture() const;
		const Texture& GetVerticalBlurTexture() const;
		const Texture& GetResultTexture() const;

	private:
		struct SettingsData
		{
			float screenWidth;
			float screenHeight;
			float mutiplier;
			float padding;
		};

		using SettingsBuffer = TypedConstantBuffer<SettingsData>;
		SettingsBuffer mSettingsBuffer;

		RenderTarget mHorizontalBlurRenderTarget;
		RenderTarget mVerticalBlurRenderTarget;

		VertexShader mVertexShader;
		PixelShader mHorizontalBlurPixelShader;
		PixelShader mVerticalBlurPixelShader;

		Sampler mSampler;

		const Texture* mSourceTexture = nullptr;
		int mBlurIterrations = 1;
		float mBlurSaturation = 1.0f;
	};
}