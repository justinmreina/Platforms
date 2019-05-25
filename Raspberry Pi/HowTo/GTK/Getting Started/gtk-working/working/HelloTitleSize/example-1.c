#include <gtk/gtk.h>



static void activate(GtkApplication* app, gpointer user_data) {

  char *title = "My Window Application";

  int panel_width = 400;
  int panel_height = 600;

  GtkWidget *window;

  window = gtk_application_window_new (app);

  //Title
  gtk_window_set_title (GTK_WINDOW (window), title);

  //Size
  gtk_window_set_default_size(GTK_WINDOW (window), panel_width, panel_height);

  //Location
  gtk_window_move(GTK_WINDOW (window), 150, 50);

  gtk_widget_show_all (window);
}


int main (int argc, char **argv) {

  GtkApplication *app;

  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);

  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return status;
}

