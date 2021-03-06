#include "MainWindow.h"
using namespace IAPDUSB;

void MainWindow::InitializeComponent()
{
	refreshTimer = gcnew Timer;
	refreshTimer->Interval = TIMER_TICK_TIME;
	refreshTimer->Enabled = true;
	refreshTimer->Tick += gcnew System::EventHandler(this, &MainWindow::Timer_Tick);
	list = gcnew ListView;
	list->Width = LISTVIEW_WIDTH;
	list->Height = LISTVIEW_HEIGHT;
	list->View = View::Details;
	list->Columns->Add(LISTVIEW_COL1, LISTVIEW_COL1_WIDTH, HorizontalAlignment::Center);
	list->Columns->Add(LISTVIEW_COL2, LISTVIEW_COL2_WIDTH, HorizontalAlignment::Center);
	list->Columns->Add(LISTVIEW_COL3, LISTVIEW_COL3_WIDTH, HorizontalAlignment::Left);
	list->Columns->Add(LISTVIEW_COL4, LISTVIEW_COL4_WIDTH, HorizontalAlignment::Left);
	list->Columns->Add(LISTVIEW_COL5, LISTVIEW_COL5_WIDTH, HorizontalAlignment::Left);
	list->Location = Point(LISTVIEW_X, LISTVIEW_Y);
	list->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::Selected_Changed);
	this->components = gcnew System::ComponentModel::Container();
	this->Size = System::Drawing::Size(WINDOW_WIDTH, WINDOW_HEIGHT);
	this->Text = WINDOW_NAME;
	this->Padding = System::Windows::Forms::Padding(0);
	this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	Controls->Add(list);

	ejectButton = gcnew Button();
	ejectButton->Text = BUTTON_TEXT;
	ejectButton->Location = Point(BUTTON_X, BUTTON_Y);
	ejectButton->Click += gcnew System::EventHandler(this, &MainWindow::Button_Click);
	Controls->Add(ejectButton);
	RefreshListView();
}

MainWindow::~MainWindow()
{
	if (components)
	{
		delete components;
	}
}

MainWindow::MainWindow()
{
	InitializeComponent();
}

void MainWindow::RefreshListView()
{
	USBEnumerator enumerator;
	int selectedIndex = this->GetSelectedIndex();
	
	list->Items->Clear();
	vector <USBDevice> devices = enumerator.getVectorOfDevices();
	for (int i = 0; i < devices.size(); i++)
	{
		String^ name = gcnew String(devices.at(i).getName().c_str());
		String^ letter = gcnew String(devices.at(i).getSymbPath().c_str());
		string total = std::to_string(devices.at(i).getTotalSpace());
		String^ totalManaged = gcnew String(total.c_str());
		string free = std::to_string(devices.at(i).getFreeSpace());
		String^ freeManaged = gcnew String(free.c_str());
		string busy = std::to_string(devices.at(i).getBusySpace());
		String^ busyManaged = gcnew String(busy.c_str());
		ListViewItem ^item = gcnew ListViewItem(name);
		item->SubItems->Add(letter);
		item->SubItems->Add(totalManaged);
		item->SubItems->Add(freeManaged);
		item->SubItems->Add(busyManaged);
		list->Items->Add(item);
	}
	WindowsPortableDevice wpd;
	DWORD count = wpd.getDeviceCount();
	PORTABLE_DEVICES *portable = wpd.getDevices();
	for (int i = 0; i < count; i++)
	{
		if (portable[i].deviceName == L"" && portable[i].count == 0)
			continue;
		wstring deviceName = portable[i].deviceName;
		String^ name = gcnew String(portable[i].deviceName.c_str());
		ListViewItem ^item = gcnew ListViewItem(name);
		list->Items->Add(item);
		for (int j = 0; j < portable[i].count; j++)
		{
			double totalDb = (double)portable[i].total.at(j) / 1000000;
			double freeDb = (double)portable[i].free.at(j) / 1000000;
			double busyDb = (double)totalDb - freeDb;
			wstring tempName = deviceName + L" : " + portable[i].storName.at(j);
			String^ storname = gcnew String(tempName.c_str());
			string total = std::to_string(totalDb);
			String^ totalManaged = gcnew String(total.c_str());
			string free = std::to_string(freeDb);
			String^ freeManaged = gcnew String(free.c_str());
			string busy = std::to_string(busyDb);
			String^ busyManaged = gcnew String(busy.c_str());
			ListViewItem ^storeItem = gcnew ListViewItem(storname);
			storeItem->SubItems->Add("");
			storeItem->SubItems->Add(totalManaged);
			storeItem->SubItems->Add(freeManaged);
			storeItem->SubItems->Add(busyManaged);
			list->Items->Add(storeItem);
		}
	}
	try {
		list->Items[selectedIndex]->Selected = true;
	}
	catch (...)
	{
		selectedIndex = 0;
	}

	return;
}

void MainWindow::Timer_Tick(System::Object^ Sender, EventArgs ^e)
{
	RefreshListView();
}

void MainWindow::Button_Click(System::Object^ Sender, EventArgs ^e)
{
	int selected = this->GetSelectedIndex();
	String ^str = list->Items[selected]->SubItems[1]->Text;
	CHAR symbPath = static_cast<char>(str[0]);
	USBDevice::ejectDrive(symbPath);
	int as = 1; 
}

int MainWindow::GetSelectedIndex()
{
	int selectedIndex = -1;
	if (list->SelectedItems->Count > 0)
	{
		selectedIndex = list->Items->IndexOf(list->SelectedItems[0]);
	}
	return selectedIndex;
}

void MainWindow::Selected_Changed(System::Object^ Sender, EventArgs ^e)
{
	int index = GetSelectedIndex();
	try
	{
		String ^selected = list->Items[index]->SubItems[1]->Text;
		if (selected == "")
		{
			ejectButton->Enabled = false;
			return;
		}
	}
	catch (...)
	{
		ejectButton->Enabled = false;
		return;
	}
	ejectButton->Enabled = true;
	return;
}