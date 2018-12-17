#include "pch.h"
#include <list>
#include "dxxamlapp1Main.h"
#include "Common\DirectXHelper.h"

std::list<wchar_t*> stdout_cust;

using namespace dxxamlapp1;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
dxxamlapp1Main::dxxamlapp1Main(const std::shared_ptr<DX::DeviceResources>& deviceResources, double swapHeight) :
	m_deviceResources(deviceResources)
{
	// Register to be notified if the Device is lost or recreated
	m_deviceResources->RegisterDeviceNotify(this);

	m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new
		SampleFpsTextRenderer(m_deviceResources, swapHeight));

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

dxxamlapp1Main::~dxxamlapp1Main()
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

void dxxamlapp1Main::StartRenderLoop()
{
	// If the animation render loop is already running then do not start another thread.
	if (m_renderLoopWorker != nullptr && m_renderLoopWorker->Status == AsyncStatus::Started)
	{
		return;
	}

	// Create a task that will be run on a background thread.
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		// Calculate the updated frame and render once per vertical blanking interval.
		while (action->Status == AsyncStatus::Started)
		{
			critical_section::scoped_lock lock(m_criticalSection);
			Update();
			if (Render())
			{
				m_deviceResources->Present();
			}
		}
	});

	// Run task on a dedicated high priority background thread.
	m_renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void dxxamlapp1Main::StopRenderLoop()
{
	m_renderLoopWorker->Cancel();
}

// Updates the application state once per frame.
void dxxamlapp1Main::Update() 
{
	// Update scene objects.
	m_timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		m_fpsTextRenderer->Update(&stdout_cust);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool dxxamlapp1Main::Render() 
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	m_fpsTextRenderer->Render();

	return true;
}

// Notifies renderers that device resources need to be released.
void dxxamlapp1Main::OnDeviceLost()
{
	m_fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void dxxamlapp1Main::OnDeviceRestored()
{
	m_fpsTextRenderer->CreateDeviceDependentResources();
}
