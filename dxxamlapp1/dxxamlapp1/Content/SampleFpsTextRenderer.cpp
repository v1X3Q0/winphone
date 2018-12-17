#include "pch.h"
#include "SampleFpsTextRenderer.h"
#include <list>

#include "Common/DirectXHelper.h"

using namespace dxxamlapp1;
using namespace Microsoft::WRL;

// Initializes D2D resources used for text rendering.
SampleFpsTextRenderer::SampleFpsTextRenderer(const
	std::shared_ptr<DX::DeviceResources>& deviceResources, double swapHeight) : 
	m_text(L""),
	m_deviceResources(deviceResources),
	m_fontSize(12.0f)
{
	ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));
	m_offsetRender = swapHeight + m_fontSize;
	// Create device independent resources
	ComPtr<IDWriteTextFormat> textFormat;
	DX::ThrowIfFailed(
		m_deviceResources->GetDWriteFactory()->CreateTextFormat(
			L"Noto Mono",
			nullptr,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			m_fontSize,
			L"en-US",
			&textFormat
			)
		);

	DX::ThrowIfFailed(
		textFormat.As(&m_textFormat)
		);

	DX::ThrowIfFailed(
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		);

	DX::ThrowIfFailed(
		m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock)
		);

	CreateDeviceDependentResources();
}

// Updates the text to be displayed.
void SampleFpsTextRenderer::Update(std::list<wchar_t*> *stdout_cust)
{
	// Update display text.
	//uint32 fps = timer.GetFramesPerSecond();
	static std::mutex m;
	std::lock_guard<std::mutex> m2(m);
	
	//m_text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";
	wchar_t cwd[0x100];
	RtlZeroMemory(cwd, 0x100 * 2);
	GetCurrentDirectoryW(0x100, cwd);
	m_text = std::wstring(cwd ) + std::wstring(L"\n");
	unsigned a_var = (unsigned)m_text.data();
	if (stdout_cust->size() == 0)
		m_text += (L"empty output\n");
	//else if (stdout_cust->size() == 1)
	//	m_text += std::to_wstring(a_var) + L"\n";
	else
	{
		if (stdout_cust->size() > 8)
		{
			//free((stdout_cust->front()));
			auto tmp = stdout_cust->back();
			stdout_cust->pop_back();
			delete[] tmp;
		}
		for (auto i = stdout_cust->begin(); i != stdout_cust->end(); i++)
		{
			m_text += std::wstring((wchar_t*)(*i)) + L"\n";
		}
	}


	ComPtr<IDWriteTextLayout> textLayout;
	DX::ThrowIfFailed(
		m_deviceResources->GetDWriteFactory()->CreateTextLayout(
			m_text.c_str(),
			(uint32) m_text.length(),
			m_textFormat.Get(),
			500.0f, // Max width of the input text.
			40.0f, // Max height of the input text.
			&textLayout
			)
		);

	DX::ThrowIfFailed(
		textLayout.As(&m_textLayout)
		);

	DX::ThrowIfFailed(
		m_textLayout->GetMetrics(&m_textMetrics)
		);
}

// Renders a frame to the screen.
void SampleFpsTextRenderer::Render()
{
	ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

	context->SaveDrawingState(m_stateBlock.Get());
	context->BeginDraw();

	// Position on the bottom right corner
	D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
		0/*logicalSize.Width - m_textMetrics.layoutWidth*/,
		m_offsetRender/*logicalSize.Height - m_textMetrics.height*/
		);

	context->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());

	DX::ThrowIfFailed(
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING/*DWRITE_TEXT_ALIGNMENT_TRAILING*/)
		);

	context->DrawTextLayout(
		D2D1::Point2F(0.f, 0.f),
		m_textLayout.Get(),
		m_whiteBrush.Get()
		);

	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
	// is lost. It will be handled during the next call to Present.
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		DX::ThrowIfFailed(hr);
	}

	context->RestoreDrawingState(m_stateBlock.Get());
}

void SampleFpsTextRenderer::CreateDeviceDependentResources()
{
	DX::ThrowIfFailed(
		m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_whiteBrush)
		);
}
void SampleFpsTextRenderer::ReleaseDeviceDependentResources()
{
	m_whiteBrush.Reset();
}