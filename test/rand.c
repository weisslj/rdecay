#include <stdio.h>
#include <string.h>
#include <glib.h>

void randomize_string(gchar *string, gint32 number)
{
    GRand *rand;
    gint32 i, rand_i, *free_numbers;
    gchar *rand_string;
    
    rand = g_rand_new();

    free_numbers = (gint32 *) g_malloc(number * sizeof(gint32));
    for (i = 0; i < number; i++)
        *(free_numbers + i) = i;

    rand_string = (gchar *) g_malloc(number * sizeof(gchar));
    for (i = number - 1; i >= 0; i--) {
        rand_i = g_rand_int_range(rand, 0, i + 1);
        *(rand_string + *(free_numbers + rand_i)) = *(string + i);
        *(free_numbers + rand_i) = *(free_numbers + i);
    }

    for (i = 0; i < number; i++)
        *(string + i) = *(rand_string + i);
 
    g_free(free_numbers);
    g_free(rand_string);
    g_rand_free(rand);
}

gint main()
{
    gchar *ctext = "butterbrot";
    gchar *text;
    gint32 x, i, length;
    
    length = strlen(ctext);
    text = (gchar *) g_malloc(length * sizeof(gchar));
    for (i = 0; i < length; i++)
        *(text + i) = *(ctext + i);

    for (x = 0; x < 32768; x++) {
        randomize_string(text, length);
        fwrite(text, sizeof(gchar), length, stdout);
        fputc('\n', stdout);
    }

    g_free(text);

    return 0;
}
