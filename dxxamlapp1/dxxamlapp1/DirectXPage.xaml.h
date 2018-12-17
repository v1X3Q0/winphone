//
// DirectXPage.xaml.h
// Declaration of the DirectXPage class.
//

#pragma once

#include "DirectXPage.g.h"

#include "Common\DeviceResources.h"
#include "dxxamlapp1Main.h"

namespace dxxamlapp1
{
	/// <summary>
	/// A page that hosts a DirectX SwapChainPanel.
	/// </summary>
	public ref class DirectXPage sealed
	{
	public:
		DirectXPage();
		virtual ~DirectXPage();

		void SaveInternalState(Windows::Foundation::Collections::IPropertySet^ state);
		void LoadInternalState(Windows::Foundation::Collections::IPropertySet^ state);

	private:
		// XAML low-level rendering event handler.
		void OnRendering(Platform::Object^ sender, Platform::Object^ args);

		// Window event handlers.
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);

		// DisplayInformation event handlers.
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

		// Other event handlers.
		void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel^ sender, Object^ args);
		void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);

		// Resources used to render the DirectX content in the XAML page background.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::unique_ptr<dxxamlapp1Main> m_main; 
		bool m_windowVisible;

		//HANDLE OUR ENTER KEY TO SUBMIT COMMAND
		void OnKeyDownHandler(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
	};
}

