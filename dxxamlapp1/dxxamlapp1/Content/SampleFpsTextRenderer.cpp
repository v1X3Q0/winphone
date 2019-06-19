#include "pch.h"
#include "SampleFpsTextRenderer.h"
#include <list>
#include "util.h"
#include "Common/DirectXHelper.h"

using namespace dxxamlapp1;
using namespace Microsoft::WRL;
std::mutex m;

#define OUT_SIZE		11
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
void SampleFpsTextRenderer::Update(std::list<char*> *stdout_cust, FILE* stdout_stream)
{
	// Update display text.
	//uint32 fps = timer.GetFramesPerSecond();
	static std::mutex m3;
	//std::lock_guard<std::mutex> m2(m3);
	while (m3.try_lock() == false);
	char* line;;
	int stdoutLoc = ftell(stdout_stream);
	int cur = 0;
	fseek(stdout_stream, 0, SEEK_SET);
	while (cur < stdoutLoc)
	{
		line = (char*)calloc(128, 1);
		fgets(line, 128, stdout_stream);
		if (strlen(line) == 0)
		{
			free(line);
			break;
		}
		cur += strlen(line);

		//std::string anew(line);

		//std::wstring newLineT = utf16_encode(std::string(line));
		//int size_alloc = newLineT.size() + 1;
		//wchar_t* anew = new wchar_t[size_alloc]/*(wchar_t*)malloc(size_alloc * 2)*/;
		//RtlZeroMemory(anew, size_alloc * 2);
		//memcpy(anew, (void*)newLineT.data(), size_alloc * 2);
		stdout_cust->push_front(line);
	}
	//errno_t err = fopen_s(&stdout_stream, "C:\\Users\\mariomain\\Pictures\\stdout_stream.txt", "w");
	//if (err == 0)
	//	cur = 1;
	//fseek(stdout_stream, 0, SEEK_SET);
	resetStream(&stdout_stream);
	//m_text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";
	wchar_t cwd[0x100];
	RtlZeroMemory(cwd, 0x100 * 2);
	GetCurrentDirectoryW(0x100, cwd);
	m_text = std::wstring(cwd) + std::wstring(L"\n");
	//unsigned a_var = (unsigned)m_text.data();
	std::string m_textTmp = "";
	if (stdout_cust->size() == 0)
		m_text += (L"empty output\n");
	//else if (stdout_cust->size() == 1)
	//	m_text += std::to_wstring(a_var) + L"\n";
	else
	{
		if (stdout_cust->size() > OUT_SIZE)
		{
			//free((stdout_cust->front()));
			char* tmp = stdout_cust->back();
			stdout_cust->pop_back();
			free(tmp);
		}
		auto last8 = stdout_cust->end();
		last8--;
		for (int i = 0; i < OUT_SIZE; i++)
		{
			m_textTmp += std::string(*last8);
			if (last8 == stdout_cust->begin())
				break;
			last8--;
			//m_text += std::wstring(stdout_cust[i] + L"\n");
		}
		m_text += std::wstring(utf16_encode(m_textTmp.data()));
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
	m3.unlock();
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