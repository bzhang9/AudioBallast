#pragma once
#include <windows.h>
#include <d2d1.h>
#include "BaseWnd.h"
#include <vector>

struct IAudioSessionManager2;
struct ID2D1Factory;
struct ID2D1HwndRenderTarget;
struct ID2D1SolidColorBrush;

class AudioControl;

class MainWnd : public BaseWnd<MainWnd> {
	IAudioSessionManager *volManager;
	IAudioSessionManager2 *manager;
	IAudioSessionNotification *sessListener;
	std::vector<AudioControl *> sessions;

	// D2D members
	ID2D1Factory *d2Factory;
	ID2D1HwndRenderTarget *d2RenderTgt;
	ID2D1SolidColorBrush *d2Brush;

	// User Input
	D2D1_POINT_2F mouseCoords;
	AudioControl *current;

	HRESULT createGraphicsResources();
	void discardGraphicsResources();
	void onPaint();
	void resize();

	// User Input helpers
	void onLDown(int xCoord, int yCoord, DWORD flags);
	void onLUp();
	void onMouseMove(int xCoord, int yCoord, DWORD flags);
	AudioControl *getController(D2D1_POINT_2F &);
	// TODO: conversion DIP <-> pixel

	D2D1_ROUNDED_RECT *createRoundRect(const D2D1_RECT_F &);
	void setControlElements();
	void paintController(AudioControl *);

public:
	MainWnd();
	MainWnd(IAudioSessionManager2 *);
	~MainWnd();

	PCWSTR ClassName() const;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	AudioControl *addSession(IAudioSessionControl *);
};