#include "GtkGUI.h"


GtkGUI::GtkGUI()
{
	int status;
	app = gtk_application_new("gtk.app", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
}


GtkGUI::~GtkGUI()
{
}


void GtkGUI::activate(GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;
	GtkTreeStore *treeStore = gtk_tree_store_new(N_COLUMNS,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_INT64,
		G_TYPE_INT64,
		G_TYPE_INT64);
	GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(treeStore));

	window = gtk_application_window_new(app);
	gtk_container_add(GTK_CONTAINER(window), tree);


	/* The view now holds a reference.  We can get rid of our own
	* reference */
	g_object_unref(G_OBJECT(treeStore));

	/* Create a cell render and arbitrarily make it red for demonstration
	* purposes */
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

	/* Create a column, associating the "text" attribute of the
	* cell_renderer to the first column of the model */
	GtkTreeViewColumn *column1 = gtk_tree_view_column_new_with_attributes(COL_NAME, renderer,
		"name", NAME_COLUMN,
		NULL);
	GtkTreeViewColumn *column2 = gtk_tree_view_column_new_with_attributes(COL_LETTER, renderer,
		"letter", LETTER_COLUMN,
		NULL);
	GtkTreeViewColumn *column3 = gtk_tree_view_column_new_with_attributes(COL_TOTAL, renderer,
		"totalSpace", TOTAL_SPACE_COLUMN,
		NULL);
	GtkTreeViewColumn *column4 = gtk_tree_view_column_new_with_attributes(COL_FREE, renderer,
		"freeSpace", FREE_SPACE_COLUMN,
		NULL);
	GtkTreeViewColumn *column5 = gtk_tree_view_column_new_with_attributes(COL_BUSY, renderer,
		"busySpace", BUSY_SPACE_COLUMN,
		NULL);
	setWidgetProps(window, column1, column2, column3, column4, column5);
	/* Add the column to the view. */
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column1);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column2); 
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column3);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column4);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column5);
	gtk_widget_show_all(window);
}


int GtkGUI::startApp(int argc, char *argv[])
{
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
}

void GtkGUI::setWidgetProps(GtkWidget *w, GtkTreeViewColumn *c1, GtkTreeViewColumn *c2, GtkTreeViewColumn *c3, GtkTreeViewColumn *c4, GtkTreeViewColumn *c5)
{
	gtk_window_set_title(GTK_WINDOW(w), MAIN_WINDOW_TITLE);
	gtk_widget_set_size_request(w, MAIN_WINDOWS_WIDTH, MAIN_WINDOW_HEIGHT);
	gtk_window_set_resizable(GTK_WINDOW(w), FALSE);

	g_object_set(c1, "min-width", TREE_NAME);
	g_object_set(c1, "resizable", TRUE);
	g_object_set(c2, "min-width", TREE_LETTER);
	g_object_set(c3, "min-width", TREE_TOTAL);
	g_object_set(c3, "resizable", TRUE);
	g_object_set(c4, "min-width", TREE_FREE);
	g_object_set(c4, "resizable", TRUE);
	g_object_set(c5, "min-width", TREE_BUSY);
	g_object_set(c5, "resizable", TRUE);
}