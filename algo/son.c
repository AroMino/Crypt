#include <stdio.h>
#include <gtk/gtk.h>
#include <pthread.h>

void* son(){
	system("./son.sh");
	return NULL;
}

void* spinstart(gpointer spin){
	gtk_spinner_start(GTK_SPINNER(spin));
	return NULL;
}

void start(GtkWidget * bouton, gpointer spin){
	pthread_t thread1, thread2;
	pthread_create(&thread1, NULL, son, NULL);
	pthread_create(&thread2, NULL, spinstart, spin);
	//pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
}

void stop(GtkWidget * bouton, gpointer spin){
	son();
	gtk_spinner_stop(GTK_SPINNER(spin));
}

int main(int argc,char** argv){
	gtk_init(&argc,&argv);
	GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget * bouton1 = gtk_button_new_with_label("start");
	GtkWidget * bouton2 = gtk_button_new_with_label("stop");
	
	GtkWidget * spin = gtk_spinner_new();
	
	GtkWidget * con = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window),con);
	
	gtk_fixed_put(GTK_FIXED(con),bouton1,0,30);
	gtk_fixed_put(GTK_FIXED(con),bouton2,90,30);
	gtk_fixed_put(GTK_FIXED(con),spin,70,10);
	
	g_signal_connect(bouton1,"clicked",G_CALLBACK(start),spin);
	g_signal_connect(bouton2,"clicked",G_CALLBACK(stop),spin);
	
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
