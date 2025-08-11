#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <glib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Directorio base para almacenamiento persistente (cámbialo si quieres)
    const char *base_dir = g_build_filename(g_get_home_dir(), ".youtube_viewer", NULL);

    // Crear directorios necesarios
    g_mkdir_with_parents(base_dir, 0700);

    gchar *cache_dir = g_build_filename(base_dir, "cache", NULL);
    gchar *data_dir  = g_build_filename(base_dir, "data", NULL);
    gchar *local_storage_dir = g_build_filename(base_dir, "localstorage", NULL);

    g_mkdir_with_parents(cache_dir, 0700);
    g_mkdir_with_parents(data_dir, 0700);
    g_mkdir_with_parents(local_storage_dir, 0700);

    // Construir un WebKitWebsiteDataManager con rutas personalizadas
    WebKitWebsiteDataManager *wdm = webkit_website_data_manager_new(
        "base-data-directory", data_dir,
        "base-cache-directory", cache_dir,
        "local-storage-directory", local_storage_dir,
        NULL
    );

    // Crear WebContext usando ese WebsiteDataManager
    WebKitWebContext *context = webkit_web_context_new_with_website_data_manager(wdm);

    // Obtener el gestionador de cookies y hacer que persistan en disco (SQLite)
    WebKitCookieManager *cookie_manager = webkit_website_data_manager_get_cookie_manager(wdm);
    // ruta del fichero de cookies
    gchar *cookie_file = g_build_filename(data_dir, "cookies.sqlite", NULL);
    webkit_cookie_manager_set_persistent_storage(cookie_manager,
                                                cookie_file,
                                                WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);
    // Opcional: aceptar todas las cookies (puedes cambiar a NO_THIRD_PARTY si prefieres)
    webkit_cookie_manager_set_accept_policy(cookie_manager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

    // Crear ventana y WebView con el contexto personalizado
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
    gtk_window_set_title(GTK_WINDOW(window), "YouTube Viewer");

    WebKitWebView *webview = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(context));
    webkit_web_view_load_uri(webview, "https://www.youtube.com");

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(webview));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    // Liberar strings construidos
    g_free(cache_dir);
    g_free(data_dir);
    g_free(local_storage_dir);
    g_free(cookie_file);
    g_free((gchar*)base_dir);

    gtk_main();
    return 0;
}
