#include "gui.h"
#include "convolution.h"
#include "objects_generator.h"

static GtkBuilder *builder;
static const gchar *ui_path = "res/app.glade";
static ObjectsMap *map;

#define MENU_ITEM_SET_ACTIVE(builder, item, state);

static void
on_generate_objects (GSimpleAction *action,
                     GVariant *variant,
                     gpointer data)
{
  generate_new_objects(map);
}

static void
on_generate_map (GSimpleAction *action,
                 GVariant *variant,
                 gpointer data)
{
  GtkImage *image;
  GdkPixbuf *pbuf;

  if (map->objects == NULL ||
      map->n_of_objects == 0)
    generate_new_objects(map);
  generate_new_map(map);

  image = GTK_IMAGE(data);
  pbuf = get_map_as_pixbuf(map);
  gtk_image_set_from_pixbuf(image, pbuf);

  g_object_unref(pbuf);
}

static GActionEntry win_entries[] =
{
  {"generate-objects", on_generate_objects, NULL, NULL, NULL}
};

static GActionEntry img_entries[] =
{
  {"generate-map",     on_generate_map,   NULL, NULL, NULL}
};

static void
on_find_objects(GtkButton *button,
                GtkBuilder *builer)
{
  GtkImage *image;
  GdkPixbuf *pbuf;

  g_assert (map->objects != NULL &&
      map->n_of_objects != 0 &&
      map->map != NULL);

  image = GTK_IMAGE(gtk_builder_get_object(builder, "image"));
  //  pbuf = find_objects (map);
  pbuf = find_objects_dft(map);
  gtk_image_set_from_pixbuf (image, pbuf);

  g_object_unref (pbuf);
}

static void
on_open_file(GtkFileChooserButton *button,
             GtkBuilder *builder)
{
  const gchar *file_name;
  GtkImage *image;

  image = GTK_IMAGE(gtk_builder_get_object(builder, "image"));
  file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));
  gtk_image_set_from_file(image, file_name);
}

static void
add_action_entries (GtkBuilder *builder)
{
  GtkApplicationWindow *window;
  GtkImage *image;

  image = GTK_IMAGE(gtk_builder_get_object(builder, "image"));
  window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(builder,
                                                         "mainwindow"));

  g_action_map_add_action_entries(G_ACTION_MAP(window),
                                  win_entries, G_N_ELEMENTS(win_entries),
                                  window);
  g_action_map_add_action_entries(G_ACTION_MAP(window),
                                  img_entries, G_N_ELEMENTS(img_entries),
                                  image);
}

static void
setup_menu (GtkBuilder *builder)
{
  GtkWidget *menu_button;
  GMenu *menu;

  menu_button = GTK_WIDGET(gtk_builder_get_object(builder,
                                                  "menubutton"));
  menu = g_menu_new();
  g_menu_append(menu, "Сгенерировать объекты", "win.generate-objects");
  g_menu_append(menu, "Сгенерировать компоновку", "win.generate-map");
  gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_button), G_MENU_MODEL(menu));
}

static void
init_buttons_signals(GtkBuilder *builder)
{
  GtkWidget *find_button;
  GtkWidget *open_button;

  open_button = GTK_WIDGET(gtk_builder_get_object(builder, "openbutton"));
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(open_button),
                                      g_get_home_dir());
  g_signal_connect(GTK_FILE_CHOOSER_BUTTON(open_button), "file-set",
                   G_CALLBACK(on_open_file), builder);

  find_button = GTK_WIDGET(gtk_builder_get_object(builder, "findbutton"));
  g_signal_connect(GTK_BUTTON(find_button), "clicked",
                   G_CALLBACK(on_find_objects), builder);
}

void on_startup(GtkApplication *app,
                gpointer data)
{
  builder = gtk_builder_new_from_file(ui_path);

  init_buttons_signals(builder);
  init_objects_map(&map);
  add_action_entries(builder);
  setup_menu(builder);
}

void on_activate(GtkApplication *app,
                 gpointer data)
{
  GtkWidget *window;

  window = GTK_WIDGET(gtk_builder_get_object(builder,
                                             "mainwindow"));
  gtk_application_add_window(app, GTK_WINDOW(window));
  gtk_widget_show_all(window);

}

void on_shutdown(GtkApplication *app,
                 gpointer data)
{
  g_object_unref(builder);
  release_objects_map(&map);
}
