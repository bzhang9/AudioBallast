#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <audiopolicy.h>
#include "MainWnd.h"
#include "BaseWnd.h"
#include "State.h"
#include "SessionListener.h"
#include "AudioControl.h"
#include "AudioBallastHelpers.h"
#include "PxlConverter.h"

#include <iostream>

// TODO: saferelease
// TODO: mute
// TODO: master vol
// TODO: session labels
// TODO: listeners (add, remove, extern update)

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}

MainWnd::MainWnd() : 
	volManager{ nullptr },
	manager{ nullptr }, 
	sessListener{ nullptr }, 
	sessions{ std::vector<AudioControl *>{} }, 
	d2Factory{ nullptr },
	d2RenderTgt{ nullptr },
	d2Brush{ nullptr },
	mouseCoords( D2D1::Point2F() ),
	current{ nullptr }
{}

MainWnd::MainWnd(IAudioSessionManager2 *mng) : 
	volManager{ nullptr },
	manager{ mng }, 
	sessListener{ new SessionListener{m_hwnd} }, 
	sessions{ std::vector<AudioControl *>{} },
	d2Factory{ nullptr },
	d2RenderTgt{ nullptr },
	d2Brush{ nullptr },
	mouseCoords( D2D1::Point2F() ),
	current{ nullptr }
{
	HRESULT result = manager->QueryInterface(__uuidof(IAudioSessionManager), (void **)&volManager);
	if (FAILED(result)) {
		return;
	}
	result = manager->RegisterSessionNotification(sessListener);
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
	SafeRelease(&manager);
}

PCWSTR MainWnd::ClassName() const {
	return L"AudioBallast";
}

LRESULT MainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	case WM_LBUTTONDOWN:
		onLDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;
	case WM_LBUTTONUP:
		onLUp();
		return 0;
	case WM_PAINT:
		onPaint();
		return 0;
	case WM_SIZE:
		resize();
		return 0;
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2Factory)))
		{
			return -1;  // Fail CreateWindowEx.
		}
		PxlConverter::init(d2Factory);
		return 0;
	case WM_DESTROY:
		discardGraphicsResources();
		SafeRelease(&d2Factory);
		PostQuitMessage(0);
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

	if (FAILED(hr)) {
		return;
	}

	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	d2RenderTgt->BeginDraw();

	d2RenderTgt->Clear(D2D1::ColorF(D2D1::ColorF::DarkGray));

	for (AudioControl *session : sessions) {
		const D2D1_COLOR_F bgColor = session->getBackClr();
		D2D1_ROUNDED_RECT *element = session->getElement();

		d2Brush->SetColor(bgColor);
		d2RenderTgt->FillRoundedRectangle(element, d2Brush);

		float fillHeight = session->getVolume() * (element->rect.bottom - element->rect.top);
		D2D1_RECT_F fillBase = D2D1::RectF(element->rect.left, element->rect.bottom - fillHeight, element->rect.right, element->rect.bottom);
		// TODO: replace with D2D1::RoundedRect(rect, radx, rady)
		D2D1_ROUNDED_RECT fill;
		fill.rect = fillBase;
		fill.radiusX = element->radiusX;
		fill.radiusY = element->radiusY;

		const D2D1_COLOR_F fgColor = session->getForeClr();
		d2Brush->SetColor(fgColor);
		d2RenderTgt->FillRoundedRectangle(fill, d2Brush);
	}

	hr = d2RenderTgt->EndDraw();
	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
	{
		discardGraphicsResources();
	}
	EndPaint(m_hwnd, &ps);
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

void MainWnd::onLDown(int xCoord, int yCoord, DWORD flags) {
	SetCapture(m_hwnd);
	mouseCoords = PxlConverter::toDip(xCoord, yCoord);
	current = getController(mouseCoords);

	if (!current) {
		return;
	}

	const D2D1_RECT_F &element = current->getElement()->rect;
	float adjust = 1.0f - (mouseCoords.y - element.top) / (element.bottom - element.top);
	current->setVolume(adjust);
	paintController(current);
}

void MainWnd::onLUp() {
	ReleaseCapture();
	current = nullptr;
}

void MainWnd::onMouseMove(int xCoord, int yCoord, DWORD flags) {
	if (!current || !(flags & MK_LBUTTON)) {
		return;
	}
	mouseCoords = PxlConverter::toDip(xCoord, yCoord);
	const D2D1_RECT_F &element = current->getElement()->rect;
	float adjust = 1.0f - (mouseCoords.y - element.top) / (element.bottom - element.top);
	if (adjust > 1.0f) {
		adjust = 1.0f;
	}
	else if (adjust < 0.0f) {
		adjust = 0.0f;
	}
	current->setVolume(adjust);
	paintController(current);
}

AudioControl *MainWnd::getController(D2D1_POINT_2F &pt) {
	FLOAT x = pt.x;
	FLOAT y = pt.y;
	for (AudioControl *session : sessions) {
		D2D1_RECT_F &rect = session->getElement()->rect;
		if (x > rect.left && x < rect.right && y > rect.top && y < rect.bottom) {
			return session;
		}
	}
	return nullptr;
}

D2D1_ROUNDED_RECT *MainWnd::createRoundRect(const D2D1_RECT_F &rect) {
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
	const float controlWidth = min(size.width / (sessions.size() + 1), 150.0f);
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
		float rad = min(0.1f * controlWidth, 0.1f * controlHeight);
		roundRect->radiusX = rad;
		roundRect->radiusY = rad;
		session->deleteElement();
		session->setElement(roundRect);
	}
}

void MainWnd::paintController(AudioControl *session) {
	HRESULT hr = createGraphicsResources();

	if (FAILED(hr)) {
		return;
	}

	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);

	d2RenderTgt->BeginDraw();

	// TODO: make clr const member
	const D2D1_COLOR_F baseColor = D2D1::ColorF(D2D1::ColorF::DarkGray);
	const D2D1_COLOR_F bgColor = session->getBackClr();
	D2D1_ROUNDED_RECT *element = session->getElement();

	d2Brush->SetColor(baseColor);
	//d2RenderTgt->FillRoundedRectangle(element, d2Brush);
	d2RenderTgt->FillRectangle(element->rect, d2Brush);

	d2Brush->SetColor(bgColor);
	d2RenderTgt->FillRoundedRectangle(element, d2Brush);

	float fillHeight = session->getVolume() * (element->rect.bottom - element->rect.top);
	D2D1_RECT_F fillBase = D2D1::RectF(element->rect.left, element->rect.bottom - fillHeight, element->rect.right, element->rect.bottom);
	// TODO: replace with D2D1::RoundedRect(rect, radx, rady)
	D2D1_ROUNDED_RECT fill;
	fill.rect = fillBase;
	fill.radiusX = element->radiusX;
	fill.radiusY = element->radiusY;

	const D2D1_COLOR_F fgColor = session->getForeClr();
	d2Brush->SetColor(fgColor);
	d2RenderTgt->FillRoundedRectangle(fill, d2Brush);

	hr = d2RenderTgt->EndDraw();
	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
	{
		discardGraphicsResources();
	}
	EndPaint(m_hwnd, &ps);
}