#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/ioctl.h>
#include <math.h>
//kompilowac program z -lm by dolaczac math.h
typedef enum 
{
    false,
    true
} bool;

int compare_file_names(const void *a, const void *b)
{
    return strcmp(((struct dirent *)a)->d_name, ((struct dirent *)b)->d_name);
}

int get_terminal_width()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}
void print_with_mark(const char *path, const char *file,int maxi_length) //do -F wypis 
{
    struct stat file_stat;
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, file);
    if (stat(fullpath, &file_stat) == -1)
    {
        fprintf(stderr, "stat() failed on %s: %s\n", file, strerror(errno));
        return;
    }
    char help[1024];
    strcpy(help,file);
    if (S_ISDIR(file_stat.st_mode)) {
        strcat( help, "/" );
    } else if (S_ISREG(file_stat.st_mode) && (file_stat.st_mode & S_IXUSR)) {
        strcat( help, "*" );
    } else if (S_ISLNK(file_stat.st_mode)) {
        strcat( help, "@" );
    } else if (S_ISSOCK(file_stat.st_mode)) {
        strcat( help, "=" );
    } else if (S_ISCHR(file_stat.st_mode) || S_ISBLK(file_stat.st_mode)) {
        strcat(help,"|");
    } 
    printf("%-*s" ,maxi_length+2, help);
}
void show_help()
{
    printf("Program ls do wyswietlania zawartosci katalogow\n");
    printf("Opcje:\n");
    printf("-H pomoc\n");
    printf("-l szczegolowa zawartosc\n");
    printf("-h w kombinacji z -l pokazuje rozmiry z jednostkami np. 1M, 22kB\n");
    printf("-R rekursywne ls dla wszystkich podfolderow\n");
    printf("-a nie ignoruje plikow zaczynajacych sie od . (ukryte)\n");
    printf("-F klasyfikuje katalogi i pliki wykonywalne oznaczeniami \n");
}
void print_human_readable_size(off_t size)
{
    const char *units[] = {"B", "K", "M", "G", "T"};
    int unit_index = 0;
    double human_size = (double)size;

    while (human_size >= 1024 && unit_index < 4) 
    {
        human_size /= 1024;
        unit_index++;
    }
    human_size = ceil(human_size * 10.0) / 10.0; // zaokraglij jak zaokragla ls
    if(unit_index==0)
        printf(" %4.0f%s", human_size, units[unit_index]);
    else
        printf(" %4.1f%s", human_size, units[unit_index]);
}
void print_file_details(const char *path, const struct dirent *pDirEnt,bool mark,bool human_size) 
{
    struct stat file_stat;
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, pDirEnt->d_name);

    if (stat(fullpath, &file_stat) == -1)
    {
        fprintf(stderr, "stat() failed on %s: %s\n", pDirEnt->d_name, strerror(errno));
        return;
    }

    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");

    printf(" %2lu", file_stat.st_nlink); //dowiazania

    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);
    printf(" %7s %7s", pw->pw_name, gr->gr_name);

    if(!human_size)
        printf(" %8ld", file_stat.st_size);
    else 
    {
        print_human_readable_size(file_stat.st_size);
    }
    char timebuf[80];
    struct tm *timeinfo = localtime(&file_stat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo);
    printf(" %s ", timebuf);
    if(!mark)
        printf("%s\n", pDirEnt->d_name);
    else 
    {
        print_with_mark(path,pDirEnt->d_name,strlen(pDirEnt->d_name));
        printf("\n");
    }
}

void list_directory(const char *path, bool extended,bool hidden,bool recursive,bool mark,bool human_size)
{
    struct dirent *pDirEnt;
    DIR *pDIR = opendir(path);

    if (pDIR == NULL) 
    {
        fprintf(stderr, "%s %d: opendir() failed (%s)\n", __FILE__, __LINE__, strerror(errno));
        return;
    }

    struct dirent *file_entries = NULL;
    int num_files = 0;
    long long int total_size = 0; //ile waza pliki

    while ((pDirEnt = readdir(pDIR))) //liczenie wagi i ilosci plikow w katalogu
    {
        if (pDirEnt->d_name[0]=='.' && !hidden) 
            continue;
        else
        {
            num_files++;
            struct stat file_stat;
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, pDirEnt->d_name);
            if (stat(fullpath, &file_stat) != -1)
                total_size += file_stat.st_size;
        }
    }

    file_entries =malloc(num_files * sizeof(struct dirent));
    if (file_entries == NULL)
    {
        fprintf(stderr, "Memory alloc failed\n");
        return;
    }

    rewinddir(pDIR);
    int i = 0;
    while ((pDirEnt = readdir(pDIR))) //wrzucenie do tablicy (rozmiar policzony w wczesniejszym whilu)
    {
        if (pDirEnt->d_name[0]=='.' && !hidden) 
            continue;
        else 
        {
            file_entries[i]=*pDirEnt;
            i++;
        }
    }

    qsort(file_entries, num_files, sizeof(struct dirent), compare_file_names);

    if(recursive)
        printf("%s: \n", path); // Wypisuj na gorze nazwe dir jezeli recursive
    if(extended) //wypisz dla -l
    {
        if(!human_size)
            printf("total %lld\n", total_size/1024); //w kB
        else 
        {
            printf("total ");
            print_human_readable_size(total_size);
            printf("\n");
        }
            

        for (int j = 0; j < num_files; j++) 
        {
             print_file_details(path, &file_entries[j],mark,human_size);
        }
    }
    else //wypisz z formatowaniem bez -l
    {
        int maxi_length = 0;
        for (int i = 0; i < num_files; i++)
        {
            if (file_entries[i].d_name[0]=='.' && !hidden) 
            continue;
            int name_length = strlen(file_entries[i].d_name);
            if (name_length > maxi_length)
                maxi_length = name_length;
        }

        int term_width = get_terminal_width();

        int cols = term_width / (maxi_length + 2);
        int multiple=(num_files+cols-1)/cols;

        int count = 0;
        for (int i = 0; i < multiple; i++)
        {
            for(int j = i;j < num_files; j+=multiple)
            {
                if (file_entries[j].d_name[0]=='.' && !hidden) 
                    continue;
                if(!mark)
                    printf("%-*s", maxi_length + 2, file_entries[j].d_name);
                else //flagamark
                {
                    print_with_mark(path,file_entries[j].d_name,maxi_length);
                }
            }
          printf("\n");
        }
    }
    if(recursive)
    {
        printf("\n");
        for (int i = 0; i < num_files; i++) 
        {
            if (file_entries[i].d_type == DT_DIR && strcmp(file_entries[i].d_name, ".") != 0 && strcmp(file_entries[i].d_name, "..") != 0)
            {
                char subdir_path[1024];
                if (path[strlen(path) - 1] == '/')
                    snprintf(subdir_path, sizeof(subdir_path), "%s%s", path, file_entries[i].d_name);
                else
                    snprintf(subdir_path, sizeof(subdir_path), "%s/%s", path, file_entries[i].d_name);
                list_directory(subdir_path, extended, hidden, true,mark,human_size);
            }
        }
    }
    free(file_entries);
    closedir(pDIR);
}

int main(int argc, char *argv[]) 
{
    const int MAX_PATHS = 128; 
    const int MAX_PATH_LENGTH = 1024; 
    char paths[MAX_PATHS][MAX_PATH_LENGTH]; 
    strncpy(paths[0], ".", MAX_PATH_LENGTH - 1);
    paths[0][MAX_PATH_LENGTH - 1] = '\0';
    int num_paths = 0; 
    bool extended = false;
    bool hidden = false;
    bool recursive=false;
    bool mark = false;
    bool human_size=false;
    bool help=false;
    for (int i = 1; i < argc; i++) 
    {
        if (argv[i][0] == '-') 
        {
            for (int j = 1; j < strlen(argv[i]); j++) 
            {
                if (argv[i][j] == 'l')
                    extended = true;
                else if (argv[i][j] == 'a')
                    hidden = true;
                else if(argv[i][j]=='R')
                    recursive=true;
                else if(argv[i][j]=='F')
                    mark=true;
                else if(argv[i][j]=='h')
                    human_size=true;
                else if(argv[i][j]=='H')
                    help=true;
                else 
                {
                    fprintf(stderr, "Unknown option: -%c\n", argv[i][j]);
                    return 0;
                }
            }
        } 
        else 
        {
            strncpy(paths[num_paths], argv[i], MAX_PATH_LENGTH - 1);
            paths[num_paths][MAX_PATH_LENGTH - 1] = '\0';
            num_paths++;
        }        
    }
    if(help)
        show_help();
    //wywolywanie listowania ls po zebraniu parametrow i sciezek
    if(num_paths==1 || num_paths==0)
        list_directory(paths[0], extended,hidden,recursive,mark,human_size);
    else 
    {
        for (int i = 0; i < num_paths; i++)
        {
            printf("%s:\n", paths[i]);
            list_directory(paths[i], extended, hidden,recursive,mark,human_size);
            printf("\n");
         }
    }
    return 0;
}
