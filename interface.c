#include <gtk/gtk.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "cry.c"

typedef struct {
	GtkWidget * window;
	GtkWidget * entry_file;
	GtkWidget * entry_key;
	GtkWidget * label_error;
	GtkWidget * spin;
	GtkWidget * text_view_apercu;
	
	GtkWidget * bouton_par_defaut;
	
	int check_live;
	int spinBool;
	char* path_save;
}win;

typedef struct {
	gpointer data;
	const char *warning;
}war;

char* get_text_view(win* p);
char* get_path(win* p);
void save_view(GtkWidget * save, win* p);
int permissions_file(char* path);
int warnings(gpointer parent, const char *warning);
void SetStyleCss(GtkWidget *widget , gchar *Name_class);

static gboolean on_widget_enter(GtkWidget *widget, GdkEvent *event, win * p){
	if(gtk_widget_has_focus(p->entry_file) || gtk_widget_has_focus(p->entry_key) || gtk_widget_has_focus(p->text_view_apercu)){
		
	}
	else gtk_window_set_focus(GTK_WINDOW(p->window), NULL);

	return FALSE;
}

static gboolean on_widget_enter1(GtkWidget *widget, GdkEvent *event){
	gtk_widget_grab_focus(widget);
	return FALSE;
}

void* son(){
	system("./son.sh");
	return NULL;
}

int save_warning_window(gpointer data, win* a){
	GtkWidget *pQuestion;
	pQuestion = gtk_message_dialog_new (GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_NONE,
        "\n          Le fichier existe !\nVoulez-vous le remplacer ?");
	gtk_window_set_default_size(GTK_WINDOW(pQuestion),200,100);
	
	/// Icône 
	const gchar *icon_path = "./cry.png";
    GdkPixbuf *icon_window = gdk_pixbuf_new_from_file(icon_path, NULL);
    gtk_window_set_icon(GTK_WINDOW(pQuestion), icon_window);

	GtkDialog *dialog = GTK_DIALOG(pQuestion);
	GtkWidget * button_oui = gtk_dialog_add_button(dialog, "Oui", GTK_RESPONSE_YES);
	GtkWidget * button_non = gtk_dialog_add_button(dialog, "Non", GTK_RESPONSE_NO);
	
	g_signal_connect(button_non, "enter-notify-event", G_CALLBACK(on_widget_enter1), NULL);
	g_signal_connect(button_oui, "enter-notify-event", G_CALLBACK(on_widget_enter1), NULL);
	
	SetStyleCss(GTK_WIDGET(dialog),"window_war");
	SetStyleCss(button_non,"button_war1");
	SetStyleCss(button_oui,"button_war1");
	
	switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
		case GTK_RESPONSE_YES:
			gtk_widget_destroy(pQuestion);
			return 0;
			
		case GTK_RESPONSE_DELETE_EVENT:
		case GTK_RESPONSE_NONE:
		case GTK_RESPONSE_NO:
			gtk_widget_destroy(pQuestion);
			return 1;
	}
	return 0;
}

void save_view(GtkWidget * save, win* p){
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Enregistrer le fichier",NULL,
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "Enregistrer", GTK_RESPONSE_ACCEPT,
                                           "Annuler", GTK_RESPONSE_CANCEL,
                                           NULL);
	ici:
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gboolean exists = g_file_test(filename, G_FILE_TEST_EXISTS);
		
        if (exists){
			if(permissions_file(filename) != 6){
				if(warnings(dialog,"\nPermission non accordée") != 3) goto ici;
			}
			else{
				p->path_save = malloc(strlen(filename)*sizeof(char));
				strcpy((p->path_save),filename);
				if(save_warning_window(dialog,p) == 1) goto ici;
				else{
					char* text = get_text_view(p);
					char* path = p->path_save;
					set_text_file(&text,path,1);
					free(text);
					free(path);
					gtk_widget_destroy(dialog);
				}
			}
		}
		else{
			char* touch = calloc(50, sizeof(char));
			sprintf(touch,"touch %s",filename);
			system(touch);
			char* text = get_text_view(p);
			set_text_file(&text,filename,1);
			free(text);
        }
        g_free(filename);
    }
    else{
		gtk_widget_destroy(dialog);
	}
}

void SetStyleCss(GtkWidget *widget , gchar *Name_class){
    GtkStyleContext *style_context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(style_context , Name_class);
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_file(css_provider , g_file_new_for_path("style.css") , NULL);
    gtk_style_context_add_provider(style_context , GTK_STYLE_PROVIDER(css_provider) , GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

char* get_text_view(win* p){
	GtkTextBuffer *p_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(p->text_view_apercu));
	GtkTextIter start, end;
	gtk_text_buffer_get_start_iter(p_buf, &start);
	gtk_text_buffer_get_end_iter(p_buf, &end);

	char *text = gtk_text_buffer_get_text(p_buf, &start, &end, FALSE);
	return text;
}


gboolean start_stop_live(GtkWidget * check_live, win* p){
	gboolean is_checked = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_live));
	if(is_checked){
		gtk_label_set_text(GTK_LABEL(p->label_error),"\nMode Live");
		gtk_button_set_label(GTK_BUTTON(check_live),"Unlive");
		gtk_button_set_image(GTK_BUTTON(check_live), NULL);
		p->check_live=1;
	}
	else{
		gtk_label_set_text(GTK_LABEL(p->label_error),"\n");
		gtk_button_set_label(GTK_BUTTON(check_live),"Live");
		GtkWidget *icon4 = gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(check_live), icon4);
		p->check_live=0;
	}
	return is_checked;
}

void clear_text(GtkTextView *text_view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    gtk_text_buffer_set_text(buffer, "", -1);
}

void append_text(GtkTextView *text_view, char* text) {
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
	GtkTextIter end_iter;
	gtk_text_buffer_get_end_iter(buffer, &end_iter);
	//gtk_text_buffer_insert(buffer, &end_iter, text, -1);
	
	if (g_utf8_validate(text, -1, NULL)) {
        gtk_text_buffer_insert(buffer, &end_iter, text, -1);
    } else {
        gchar *clean_text = g_utf8_make_valid(text, -1);
        gtk_text_buffer_insert(buffer, &end_iter, clean_text, -1);
        g_free(clean_text);
    }
}

gboolean stopper_spin(win* p){
	if(p->spinBool==1){
		gtk_spinner_stop(GTK_SPINNER(p->spin));
		gtk_label_set_text(GTK_LABEL(p->label_error),"\nTerminé avec succès");
		p->spinBool=0;
	}
	if(p->check_live && p->spinBool==0) gtk_label_set_text(GTK_LABEL(p->label_error),"\nMode Live");
	return G_SOURCE_CONTINUE;
}

void select_file(GtkWidget * file, win* p){
	gtk_label_set_text(GTK_LABEL(p->label_error),"\n");
	
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

	dialog = gtk_file_chooser_dialog_new("Sélectionnez un fichier",
										 GTK_WINDOW(p->window),
										 action,
										 "Ouvrir",
										 GTK_RESPONSE_ACCEPT,
										 "Annuler",
										 GTK_RESPONSE_CANCEL,
										 NULL);

	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	if(response == GTK_RESPONSE_ACCEPT){
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		char* filename = gtk_file_chooser_get_filename(chooser);
		gtk_entry_set_text(GTK_ENTRY(p->entry_file),filename);
		g_free(filename);
	}
	gtk_widget_destroy(dialog);
}

char* get_path(win* p){
	const char* chemin = gtk_entry_get_text(GTK_ENTRY(p->entry_file));
	char* path = calloc(strlen(chemin),sizeof(char));
	strcpy(path,chemin);

	return path;
}

char* get_key(win* p){
	const char* tmp = gtk_entry_get_text(GTK_ENTRY(p->entry_key));
	char* path = calloc(strlen(tmp),sizeof(char));
	strcpy(path,tmp);
	return path;
}

void effacer_texte_view(GtkWidget * clear, win* p){
	clear_text(GTK_TEXT_VIEW(p->text_view_apercu));
	gtk_label_set_text(GTK_LABEL(p->label_error),"\n");
}

void set_texte_view(char** text,int line,win* p){
	clear_text(GTK_TEXT_VIEW(p->text_view_apercu));
	for(int i=0 ; i<line ; i++){
		append_text(GTK_TEXT_VIEW(p->text_view_apercu), text[i]);
	}
}

int warnings(gpointer data, const char *warning){
   
    GtkWidget *pQuestion;
	pQuestion = gtk_message_dialog_new (GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_NONE,
        "%s",warning);
	gtk_window_set_default_size(GTK_WINDOW(pQuestion),200,100);
	
	/// Icône
	const gchar *icon_path = "./cry.png";
    GdkPixbuf *icon_window = gdk_pixbuf_new_from_file(icon_path, NULL);
    gtk_window_set_icon(GTK_WINDOW(pQuestion), icon_window);

	GtkDialog *dialog = GTK_DIALOG(pQuestion);
	GtkWidget *button = gtk_dialog_add_button(dialog, "OK", GTK_RESPONSE_YES);
	SetStyleCss(GTK_WIDGET(dialog),"window_war");
	SetStyleCss(GTK_WIDGET(button),"button_war");
	
    // Réduire la taille du bouton
    gtk_widget_set_size_request(button, 0, 30);
	
	switch(gtk_dialog_run(GTK_DIALOG(pQuestion))){
		case GTK_RESPONSE_YES:
			gtk_widget_destroy(pQuestion);
			return 1;
			
		case GTK_RESPONSE_DELETE_EVENT:
		case GTK_RESPONSE_NONE:
		case GTK_RESPONSE_NO:
			gtk_widget_destroy(pQuestion);
			return 0;
	}
	return 0;
}

bool binary_test(char* path){
	FILE* file = fopen(path,"rb");
	int ch;
	while ((ch = fgetc(file)) != EOF) {
		if (!isprint(ch) && ch != '\n' && ch != '\r' && ch != '\t' && ch<128){
			fclose(file);
			return true;
		}
	}
	fclose(file);
    return false;
}

int permissions_file(char* path){
	struct stat info_fichier;
	int valeur = 0;
	if (stat(path, &info_fichier) == 0){  
		uid_t uid = getuid();
		gid_t gid = getgid();
		mode_t permissions = info_fichier.st_mode;
        
        if (info_fichier.st_uid == uid){
			if ((permissions & S_IRUSR) && (permissions & S_IWUSR)){
				valeur += 6;
			}
		} 
		else{
			if (info_fichier.st_gid == gid) {
				if ((permissions & S_IRGRP) && (permissions & S_IWGRP)){
					valeur +=6;
				}
			}
			else{
				if ((permissions & S_IROTH) && (permissions & S_IWOTH)){
					valeur +=6;
				}
			}
        }
	}
	return valeur;
}

int analyse_file(char* path, win* p){
	if(strlen(path)==0){
		warnings(p->window,"\nFichier manquant !");
		return 1;
	}
	else if((g_file_test(path, G_FILE_TEST_EXISTS)) == 0){
		/// introuvable
		warnings(p->window,"\nFichier introuvable !");
		return 1;
	}
	else{
		/// existe
		if(binary_test(path)){
			warnings(p->window,"\nFichier binaire !");
			return 1;
		}
		else if(permissions_file(path) == 0){
			warnings(p->window,"\nPermission non ancordée !");
			return 1;
		}
	}
	return 0;
}

void apercu_file(GtkWidget * apercu, win* p){
	char* path = NULL;
	path = get_path(p);
	
		if((analyse_file(path,p)) == 0){
			int line = getLineNumber(path);
			char** text = allouer(line,500);
			get_text_file(text,path);
			gtk_label_set_text(GTK_LABEL(p->label_error),"\nAperçu du fichier");
			set_texte_view(text,line,p);
			free(text);
		}
}

void* start_spin(gpointer spin){
	gtk_spinner_start(GTK_SPINNER(spin));
	return NULL;
}

void synchro(gpointer spin){
	pthread_t thread1, thread2;
	pthread_create(&thread1,NULL,son,NULL);
	pthread_create(&thread2,NULL,start_spin,spin);
}

void crypta(GtkWidget * crypt, win* p){
	char* path = NULL;
	char* key = NULL;
	path = get_path(p);
	key = get_key(p);
	
	if(p->check_live==0){
		if((analyse_file(path,p)) == 0){
			if(strlen(key) == 0) warnings(p->window,"\nEntrez votre clé !");
			else{
				cryptage_file(path,key);
				synchro(p->spin);
				p->spinBool=1;
			}
		}
	}
	
	else{
		if(strlen(key) == 0) warnings(p->window,"\nEntrez votre clé !");
		else{
			synchro(p->spin);
			p->spinBool=1;
			char* text = get_text_view(p);
			char* textc = calloc(strlen(text)*8 + 1,sizeof(char));
			cryptage_view(text,textc,key);
			set_texte_view(&textc,1,p);
			free(text);
			free(textc);
		}
	}
}

void decrypta(GtkWidget * decrypt, win* p){
	char* path = NULL;
	char* key = NULL;
	path = get_path(p);
	key = get_key(p);
	
	if(p->check_live==0){
		if((analyse_file(path,p)) == 0){
			if(strlen(key) == 0) warnings(p->window,"Entrez votre clé !");
			else{
				decryptage_file(path,key);
				synchro(p->spin);
				p->spinBool=1;
			}
		}
	}
	
	else{
		if(strlen(key) == 0) warnings(p->window,"Entrez votre clé !");
		else{
			synchro(p->spin);
			p->spinBool=1;
			char* text = get_text_view(p);
			char* textc = calloc(strlen(text),sizeof(char));
			decryptage_view(text,textc,key);
			set_texte_view(&textc,1,p);
			free(text);
			free(textc);
		}
	}
}

void on_entry_focused(GtkWidget *widget, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(widget);
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);
    gint text_length = gtk_entry_buffer_get_length(buffer);
    gtk_editable_set_position(GTK_EDITABLE(entry), text_length);
}

void masquer_entry_key(GtkWidget * check_hide, win* p){
	gboolean is_checked = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_hide));
	if(is_checked){
		GtkWidget *icon5 = gtk_image_new_from_icon_name("visibility", GTK_ICON_SIZE_BUTTON);
		gtk_entry_set_visibility(GTK_ENTRY(p->entry_key), FALSE);
		gtk_button_set_image(GTK_BUTTON(check_hide), icon5);
		gtk_widget_grab_focus(p->entry_key);
		on_entry_focused(p->entry_key, NULL);
		
	}
	else{
		GtkWidget *icon5 = gtk_image_new_from_icon_name("visibility", GTK_ICON_SIZE_BUTTON);
		gtk_entry_set_visibility(GTK_ENTRY(p->entry_key), TRUE);
		gtk_button_set_image(GTK_BUTTON(check_hide), icon5);
		gtk_widget_grab_focus(p->entry_key);
		on_entry_focused(p->entry_key, NULL);
	}
}

int main(int argc, char** argv){
	gtk_init(&argc, &argv);
	
/*************** Window principale *****************/
	GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Crypta");
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 425);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	SetStyleCss(window,"window");
	
/******** Icône de la fenêtre ********/
	const gchar *icon_path = "./cry.png";
    GdkPixbuf *icon_window = gdk_pixbuf_new_from_file(icon_path, NULL);
    gtk_window_set_icon(GTK_WINDOW(window), icon_window);
    
    
    
	GtkWidget * bouton_file = gtk_button_new_with_label(" File");
	GtkWidget *icon = gtk_image_new_from_stock(GTK_STOCK_FILE, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(bouton_file), icon);
	
	GtkWidget * entry_file = gtk_entry_new();
	
	GtkWidget * entry_key = gtk_entry_new();
	GtkWidget * label_key = gtk_label_new("KEY  :");
	gtk_entry_set_visibility(GTK_ENTRY(entry_key), FALSE);
	
	GtkWidget * label_error = gtk_label_new("\n");
	gtk_widget_set_size_request(entry_file, 200, 30);
	PangoFontDescription *fontDesc = pango_font_description_from_string("Arial 11");
    gtk_widget_override_font(label_error, fontDesc);
    pango_font_description_free(fontDesc);
    
	
	GtkWidget * bouton_crypt = gtk_button_new_with_label("Crypta");
	GtkWidget * bouton_decrypt = gtk_button_new_with_label("Decrypta");
	GtkWidget * bouton_terminer = gtk_button_new_with_label("Finish");
	
	gtk_widget_set_size_request(bouton_crypt, 100, 30);
	gtk_widget_set_size_request(bouton_decrypt, 100, 30);
	gtk_widget_set_size_request(bouton_terminer, 100, 30);
	
	GtkWidget * bouton_apercu = gtk_button_new_with_label("View");
	gtk_widget_set_size_request(bouton_apercu,80,30);
	GtkWidget *icon3 = gtk_image_new_from_stock(GTK_STOCK_ZOOM_100, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(bouton_apercu), icon3);
	
	GtkWidget * bouton_clear = gtk_button_new_with_label("Clear");
	gtk_widget_set_size_request(bouton_clear, 30, 30);
	GtkWidget *icon1 = gtk_image_new_from_stock(GTK_STOCK_DELETE, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(bouton_clear), icon1);
	
	GtkWidget * check_live = gtk_toggle_button_new_with_label ("Live");
	gtk_widget_set_size_request(check_live, 75, 30);
	GtkWidget *icon4 = gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(check_live), icon4);
	
	
	GtkWidget * bouton_save = gtk_button_new_with_label("Save");
	gtk_widget_set_size_request(bouton_save, 70, 30);
	GtkWidget *icon2 = gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(bouton_save), icon2);
	
	GtkWidget * scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	GtkWidget * text_view_apercu = gtk_text_view_new();
	gtk_widget_set_size_request(scrolled_window, 608, 100);
	gtk_widget_set_size_request(text_view_apercu, 400, 100);
	gtk_container_add(GTK_CONTAINER(scrolled_window), text_view_apercu);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view_apercu),5);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view_apercu),5);

	
	GtkWidget * label_vide = gtk_label_new("");
	GtkWidget * spin = gtk_spinner_new();
	
	GtkWidget * check_hide = gtk_toggle_button_new();
	GtkWidget *icon5 = gtk_image_new_from_icon_name("visibility", GTK_ICON_SIZE_BUTTON);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_hide), TRUE);
	
	gtk_widget_set_size_request(check_hide, 30, 30);
	gtk_button_set_image(GTK_BUTTON(check_hide), icon5);
	
	GtkWidget * fixe = gtk_fixed_new();
	GtkWidget * grid = gtk_grid_new();
	
/***************** Image Back *****************/
	const char *image_file = "/home/arrow/Projets/crypt/cry1.png";
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_file, NULL);
    GtkWidget * image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_fixed_put(GTK_FIXED(fixe),image,0,0);
    
    const char *image_file1 = "/home/arrow/Projets/crypt/cry2.png";
	pixbuf = gdk_pixbuf_new_from_file(image_file1, NULL);
    image = gtk_image_new_from_pixbuf(pixbuf);
    gtk_fixed_put(GTK_FIXED(fixe),image,450,80);
    
/**********************************************/
	
	GtkWidget * bouton_par_defaut = gtk_button_new();
	gtk_window_set_focus(GTK_WINDOW(window), bouton_par_defaut);
	
	gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
	
	gtk_widget_set_margin_start((grid), 10);
	gtk_widget_set_margin_top((grid), 30);
	
	gtk_widget_set_margin_end(grid, 10);
	gtk_widget_set_margin_bottom(grid, 10);
	
	gtk_container_add(GTK_CONTAINER(window), fixe);
	gtk_fixed_put(GTK_FIXED(fixe),grid,0,30);
	gtk_fixed_put(GTK_FIXED(fixe),scrolled_window,10,315);
	
	gtk_grid_attach(GTK_GRID(grid),bouton_file,1,0,1,1);
	gtk_grid_attach(GTK_GRID(grid),entry_file,2,0,3,1);
	
	
	gtk_grid_attach(GTK_GRID(grid),label_key,1,1,1,1);
	gtk_grid_attach(GTK_GRID(grid),entry_key,2,1,3,1);
	gtk_grid_attach(GTK_GRID(grid),check_hide,5,1,1,1);
	
	gtk_grid_attach(GTK_GRID(grid),label_error,2,2,3,1);
	gtk_grid_attach(GTK_GRID(grid),label_vide,0,3,4,1);
	
	gtk_grid_attach(GTK_GRID(grid),bouton_crypt,6,4,1,1);
	gtk_grid_attach(GTK_GRID(grid),bouton_decrypt,7,4,1,1);
	gtk_grid_attach(GTK_GRID(grid),bouton_terminer,7,5,1,1);
	
	gtk_grid_attach(GTK_GRID(grid),bouton_apercu,0,6,1,1);
	gtk_grid_attach(GTK_GRID(grid),bouton_clear,1,6,1,1);
	gtk_grid_attach(GTK_GRID(grid),check_live,3,6,1,1);
	gtk_grid_attach(GTK_GRID(grid),bouton_save,2,6,1,1);
	
	gtk_grid_attach(GTK_GRID(grid),spin,0,5,1,1);
	
/************************ Styles css **************************/
	GdkColor color, color2;
	gdk_color_parse("#082535",&color);
	gdk_color_parse("white",&color2);
	gtk_widget_modify_bg(text_view_apercu,GTK_STATE_NORMAL,&color);
	
	SetStyleCss(window,"window");
	SetStyleCss(entry_file,"entry");
	SetStyleCss(entry_key,"entry");
	SetStyleCss(label_key,"text");
	SetStyleCss(text_view_apercu,"text");
	
	SetStyleCss(bouton_crypt,"button");
	SetStyleCss(bouton_decrypt,"button");
	SetStyleCss(bouton_file,"button");
	SetStyleCss(bouton_apercu,"button");
	SetStyleCss(bouton_save,"button");
	SetStyleCss(bouton_clear,"button");
	SetStyleCss(bouton_terminer,"button");
	SetStyleCss(check_live,"button");
	SetStyleCss(check_hide,"button");
	
	
/********************   Signals  *************************/

	win* p = malloc(sizeof(win));
	p->window=window;
	p->entry_file=entry_file;
	p->entry_key=entry_key;
	p->label_error=label_error;
	p->spin = spin;
	p->text_view_apercu=text_view_apercu;
	p->spinBool=0;						// Status du spin
	p->check_live=0;					// Status check_live;
	
	p->bouton_par_defaut = bouton_par_defaut;
	
	g_signal_connect(bouton_file, "clicked", G_CALLBACK(select_file), p);
	g_signal_connect(bouton_crypt, "clicked", G_CALLBACK(crypta), p);
	g_signal_connect(bouton_decrypt, "clicked", G_CALLBACK(decrypta), p);
	g_signal_connect(bouton_terminer, "clicked", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(bouton_apercu, "clicked", G_CALLBACK(apercu_file), p);
	g_signal_connect(bouton_clear, "clicked", G_CALLBACK(effacer_texte_view), p);
	g_signal_connect(bouton_save, "clicked", G_CALLBACK(save_view), p);
	g_signal_connect(check_live, "clicked", G_CALLBACK(start_stop_live), p);
	g_signal_connect(check_hide, "clicked", G_CALLBACK(masquer_entry_key), p);
	
	g_signal_connect(bouton_file, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	g_signal_connect(bouton_crypt, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	g_signal_connect(bouton_decrypt, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	g_signal_connect(bouton_terminer, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	g_signal_connect(bouton_apercu, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	g_signal_connect(bouton_clear, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	g_signal_connect(bouton_save, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	g_signal_connect(check_live, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	g_signal_connect(check_hide, "enter-notify-event", G_CALLBACK(on_widget_enter), p);
	

/********* Timer du spin **********/
	g_timeout_add(1500, (GSourceFunc)stopper_spin, p);
	
	
	gtk_widget_show_all(window);
	gtk_main();
	
	return 0;
}
