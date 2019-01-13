#include <windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <audiopolicy.h>
#include "MainWnd.h"
#include "BaseWnd.h"
#include "State.h"
#include "SessionListener.h"
#include "AudioControl.h"

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
	sessions{ std::vector<AudioControl *>{} }, 
	d2Factory{ nullptr },
	d2RenderTgt{ nullptr },
	d2Brush{ nullptr }
{}

MainWnd::MainWnd(IAudioSessionManager2 *mng) : 
	manager{ mng }, 
	sessListener{ new SessionListener{m_hwnd} }, 
	sessions{ std::vector<AudioControl *>{} },
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
		addSession(session);
	}
	
}

MainWnd::~MainWnd() {
	if (sessListener) {
		HRESULT result = manager->UnregisterSessionNotification(sessListener);
		delete sessListener;
	}
	// TODO: syntax
	for (AudioControl *session : sessions) {
		delete session;
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

AudioControl *MainWnd::addSession(IAudioSessionControl *session) {
	AudioControl *control = new AudioControl{ session };
	sessions.emplace_back(control);
	setControlElements();
	return control;
}

//private methods

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
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 0, 0);
			hr = d2RenderTgt->CreateSolidColorBrush(color, &d2Brush);

			if (SUCCEEDED(hr))
			{
				setControlElements();
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

		d2RenderTgt->Clear(D2D1::ColorF(D2D1::ColorF::DarkGray));

		for (AudioControl *session : sessions) {
			d2RenderTgt->FillRoundedRectangle(session->getElement(), d2Brush);
		}

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
		setControlElements();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

D2D1_ROUNDED_RECT *MainWnd::createRoundRect(D2D1_RECT_F &rect) {
	D2D1_ROUNDED_RECT *roundRect = new D2D1_ROUNDED_RECT{};
	roundRect->rect = rect;
	float radius = min(rect.right - rect.left, rect.bottom - rect.top) / 2;
	roundRect->radiusX = radius;
	roundRect->radiusY = radius;
	return roundRect;
}

void MainWnd::setControlElements() {
	if (!d2RenderTgt) {
		return;
	}

	D2D1_SIZE_F size = d2RenderTgt->GetSize();
	const float controlWidth = size.width / (sessions.size() + 1);
	const float controlHeight = size.height * 4 / 5;
	const float topPadding = size.height / 10;

	for (int i = 0; i < sessions.size(); i++) {
		AudioControl *session = sessions[i];
		float leftEdge = 0.5f * controlWidth + (i * controlWidth);
		D2D1_RECT_F rect = D2D1::RectF(leftEdge, topPadding, leftEdge + 0.9f * controlWidth, controlHeight + topPadding);
		//D2D1_ROUNDED_RECT roundRect = D2D1::RoundedRect(rect, 0, 0);
		D2D1_ROUNDED_RECT *roundRect = new D2D1_ROUNDED_RECT{};
		//roundRect->rect = std::move(rect);
		roundRect->rect = rect;
		session->deleteElement();
		session->setElement(roundRect);
	}
}