#include <windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <audiopolicy.h>
#include "MainWnd.h"
#include "BaseWnd.h"
#include "State.h"
#include "SessionListener.h"

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

MainWnd::MainWnd() : 
	manager{ nullptr }, 
	sessListener{ nullptr }, 
	sessions{ std::vector<IAudioSessionControl *>{} }, 
	d2Factory{ nullptr },
	d2RenderTgt{ nullptr },
	d2Brush{ nullptr }
{}

MainWnd::MainWnd(IAudioSessionManager2 *mng) : 
	manager{ mng }, 
	sessListener{ new SessionListener{m_hwnd} }, 
	sessions{ std::vector<IAudioSessionControl *>{} },
	d2Factory{ nullptr },
	d2RenderTgt{ nullptr },
	d2Brush{ nullptr }
{
	HRESULT result = manager->RegisterSessionNotification(sessListener);
	if (FAILED(result)) {
		return;
	}
	IAudioSessionEnumerator *sessEnum;
	result = manager->GetSessionEnumerator(&sessEnum);
	if (FAILED(result)) {
		return;
		// TODO: throw
	}
	// iterate sessions

	IAudioSessionControl *session;
	int count;
	result = sessEnum->GetCount(&count);
	if (FAILED(result)) {
		return;
	}
	for (int i = 0; i < count; i++) {
		result = sessEnum->GetSession(i, &session);
		if (FAILED(result)) {
			continue;
		}
		sessions.emplace_back(session);
	}
	
}

MainWnd::~MainWnd() {
	if (sessListener) {
		HRESULT result = manager->UnregisterSessionNotification(sessListener);
		delete sessListener;
	}
	// TODO: syntax
	for (IAudioSessionControl *session : sessions) {
		session->Release();
	}
	manager->Release();
}

PCWSTR MainWnd::ClassName() const {
	return L"AudioBallast";
}

LRESULT MainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2Factory)))
		{
			return -1;  // Fail CreateWindowEx.
		}
		return 0;
	case WM_DESTROY:
		discardGraphicsResources();
		SafeRelease(&d2Factory);
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		onPaint();
		return 0;
		/*PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(m_hwnd, &ps);*/
	case WM_SIZE:
		resize();
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

//private methods
void MainWnd::calculateLayout()
{
	if (d2RenderTgt != nullptr)
	{
		D2D1_SIZE_F size = d2RenderTgt->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		const float radius = min(x, y);
		//ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}

HRESULT MainWnd::createGraphicsResources()
{
	HRESULT hr = S_OK;
	if (!d2RenderTgt)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = d2Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&d2RenderTgt);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			hr = d2RenderTgt->CreateSolidColorBrush(color, &d2Brush);

			if (SUCCEEDED(hr))
			{
				calculateLayout();
			}
		}
	}
	return hr;
}

void MainWnd::discardGraphicsResources()
{
	SafeRelease(&d2RenderTgt);
	SafeRelease(&d2Brush);
}

void MainWnd::onPaint()
{
	HRESULT hr = createGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		d2RenderTgt->BeginDraw();

		d2RenderTgt->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));
		//d2RenderTgt->FillEllipse(ellipse, d2Brush);

		hr = d2RenderTgt->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			discardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void MainWnd::resize()
{
	if (d2RenderTgt != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		d2RenderTgt->Resize(size);
		calculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}