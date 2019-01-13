#pragma once
#include <windows.h>
#include "BaseWnd.h"
#include <vector>

struct IAudioSessionManager2;
struct ID2D1Factory;
struct ID2D1HwndRenderTarget;
struct ID2D1SolidColorBrush;

class MainWnd : public BaseWnd<MainWnd> {
	IAudioSessionManager2 *manager;
	IAudioSessionNotification *sessListener;
	std::vector<IAudioSessionControl *> sessions;

	// D2D members
	ID2D1Factory *d2Factory;
	ID2D1HwndRenderTarget *d2RenderTgt;
	ID2D1SolidColorBrush *d2Brush;
	

	void calculateLayout();
	HRESULT createGraphicsResources();
	void discardGraphicsResources();
	void onPaint();
	void resize();

public:
	// TODO: confirm
	MainWnd();
	MainWnd(IAudioSessionManager2 *mng);
	~MainWnd();

	PCWSTR ClassName() const;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};