#pragma once
#define WINDOW_NAME "USB Info"
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 300
#define LISTVIEW_WIDTH 475
#define LISTVIEW_HEIGHT 220
#define LISTVIEW_X 5
#define LISTVIEW_Y 5
#define LISTVIEW_COL1 "Имя"
#define LISTVIEW_COL2 "Буква"
#define LISTVIEW_COL3 "Всего, МБ"
#define LISTVIEW_COL4 "Свободно, МБ"
#define LISTVIEW_COL5 "Занято, МБ"
#define LISTVIEW_COL1_WIDTH 200
#define LISTVIEW_COL2_WIDTH 60
#define LISTVIEW_COL3_WIDTH 70
#define LISTVIEW_COL4_WIDTH 70
#define LISTVIEW_COL5_WIDTH 70
namespace IAPDUSB {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MainWindow : public System::Windows::Forms::Form
	{
	public:
		MainWindow(void);

	protected:
		~MainWindow();
	private:
		ListView ^list;
		System::ComponentModel::Container ^components;
		void InitializeComponent(void);
	};
}
