#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define USAGE "Usage: %s [-b] [-c compilername] filename\n"
#else
#define USAGE "Usage: %s [-c compilername] filename\n"
#endif
const char table [16] = 
{
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
};
int main (int argc, char** argv)
{
    if (argc < 2)
    {
        printf (USAGE, argv [0]);
        exit (-1);
    }
    else
    {
        int i;
        int opt_b = 0;
        char* filename = NULL;
        char* compname = NULL;
        for (i=1; i<argc; )
        {
            if (! strcmp (argv [i], "-b"))
            {
                #ifdef _WIN32
                if (filename)
                {
                    printf ("Incorrect option position: -b\n");
                    exit (-1);
                }
                else opt_b = 1;
                #else
                printf ("Error: -b is not a valid option!\n");
                exit (-1);
                #endif
            }
            else if (! strcmp (argv [i], "-c"))
            {
                if (compname || filename)
                {
                    printf ("Incorrect option position: -c\n");
                    exit (-1);
                }
                else compname = argv [++i];
            }
            else
            {
                if (filename)
                {
                    printf ("Filename already given!\n");
                    exit (-1);
                }
                else filename = argv [i];
            }
            i++;
        }
        if (filename)
        {
            FILE* f = fopen (filename, "rb");
            char* c, t;
            FILE* o;
            c = calloc (strlen (filename) + 5, 1);
            strcpy (c, filename);
            strcat (c, ".c");
            o = fopen (c, "wb");
            fprintf (o, "#include <stdio.h>\nextern char* content;\nint main (void)\n{\n    freopen (\"%s\", \"wb\", stdout);\n"
            "    puts (content);\n    return 0;\n}\nchar* content = \"", filename);
            while (t = getc (f), t != EOF)
            {
                fprintf (o, "\\x");
                putc (table [(t & 0xf0) >> 4], o);
                putc (table [t & 0x0f], o);
            }
            fprintf (o, "\";\n");
            fclose (f);
            fclose (o);
            if (compname)
            {
                char compiler [100];
                strcpy (compiler, compname);
                strcat (compiler, " ");
                #ifdef _WIN32
                if (opt_b) strcat (compiler, "-mwindows ");
                #endif
                strcat (compiler, c);
                strcat (compiler, " -o autoextract"
                #ifdef _WIN32
                ".exe"
                #endif
                " -O3"
                );
                if (system (compiler))
                {
                    printf ("I think something wrong happened while compiling this object...\n");
                    exit (-1);
                }
            }
            exit (0);
        }
        else
        {
            printf ("Error: filename not given!\n");
            exit (-1);
        }
    }
}


