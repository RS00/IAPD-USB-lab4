#pragma once
#include <gtk\gtk.h>
#include <gdk\gdkwin32.h>
#include <Windows.h>
#include <vector>
#include "USBEnumerator.h"
#include "USBDevice.h"
#define MAIN_WINDOW_TITLE "USB Info"
#define MAIN_WINDOW_HEIGHT 300
#define MAIN_WINDOWS_WIDTH 550
#define TREE_NAME 200
#define TREE_LETTER 50
#define TREE_TOTAL 100
#define TREE_FREE 100
#define TREE_BUSY 100
#define COL_NAME "Name"
#define COL_LETTER "Letter"
#define COL_TOTAL "Total, MB"
#define COL_FREE "Free, MB"
#define COL_BUSY "Busy, MB"
using namespace std;

enum
{
	NAME_COLUMN,
	LETTER_COLUMN,
	TOTAL_SPACE_COLUMN,
	FREE_SPACE_COLUMN,
	BUSY_SPACE_COLUMN,
	N_COLUMNS
};

class GtkGUI
{
private:
	static WNDPROC OldWinProc;
	GtkApplication *app;
	GtkWidget *tree;
	static void activate(GtkApplication* app, gpointer user_data);
	static void setWidgetProps(GtkWidget *w, GtkTreeViewColumn *c1, GtkTreeViewColumn *c2, GtkTreeViewColumn *c3, GtkTreeViewColumn *c4, GtkTreeViewColumn *c5);
	static void changeWndProc(HWND hWnd);
	static LONG CALLBACK MyWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static HWND getHwndFromWindow(GtkWidget *w);
	static void refreshTreeStore(GtkTreeStore *treeStore);
public:
	GtkGUI();
	int startApp(int argc, char *argv[]);
	~GtkGUI();
};

