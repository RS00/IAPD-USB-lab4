#include "MainWindow.h"

void IAPDUSB::MainWindow::InitializeComponent()
{
	list = gcnew ListView;
	list->Width = LISTVIEW_WIDTH;
	list->Height = LISTVIEW_HEIGHT;
	list->View = View::Details;
	list->Columns->Add(LISTVIEW_COL1, LISTVIEW_COL1_WIDTH, HorizontalAlignment::Center);
	list->Columns->Add(LISTVIEW_COL2, LISTVIEW_COL2_WIDTH, HorizontalAlignment::Center);
	list->Columns->Add(LISTVIEW_COL3, LISTVIEW_COL3_WIDTH, HorizontalAlignment::Center);
	list->Columns->Add(LISTVIEW_COL4, LISTVIEW_COL4_WIDTH, HorizontalAlignment::Center);
	list->Columns->Add(LISTVIEW_COL5, LISTVIEW_COL5_WIDTH, HorizontalAlignment::Center);
	list->Location = Point(LISTVIEW_X, LISTVIEW_Y);
	this->components = gcnew System::ComponentModel::Container();
	this->Size = System::Drawing::Size(WINDOW_WIDTH, WINDOW_HEIGHT);
	this->Text = WINDOW_NAME;
	this->Padding = System::Windows::Forms::Padding(0);
	this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	Controls->Add(list);
}

IAPDUSB::MainWindow::~MainWindow()
{
	if (components)
	{
		delete components;
	}
}

IAPDUSB::MainWindow::MainWindow()
{
	InitializeComponent();
}