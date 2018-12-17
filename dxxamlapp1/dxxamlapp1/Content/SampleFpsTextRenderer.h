#pragma once

#include <string>
#include <list>
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"

namespace dxxamlapp1
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class SampleFpsTextRenderer
	{
	public:
		SampleFpsTextRenderer(const std::shared_ptr<DX::DeviceResources>&
			deviceResources, double);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(std::list<wchar_t*>*);
		void Render();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		double											m_offsetRender;
		double											m_fontSize;
		std::wstring									m_cwd;
		// Resources related to text rendering.
		std::wstring                                    m_text;
		DWRITE_TEXT_METRICS	                            m_textMetrics;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;
		Microsoft::WRL::ComPtr<IDWriteTextLayout3>      m_textLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat2>      m_textFormat;
	};
}