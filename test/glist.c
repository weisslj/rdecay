#include <stdio.h>
#include "gtk/gtk.h"

typedef struct _Numbers {
    const char *name;
    int a;
    int b;
    int c;
} Numbers;

static gint compare_name(Numbers *num, const gchar *name)
{
    return g_ascii_strcasecmp(num->name, name);
}

int main()
{
    GList *list = NULL;
    Numbers *one, *two, *three, *x;

    one = (Numbers *) g_malloc(sizeof(Numbers));
    two = (Numbers *) g_malloc(sizeof(Numbers));
    three = (Numbers *) g_malloc(sizeof(Numbers));

    one->name = "eins";
    two->name = "zwei";
    three->name = "drei";

    one->a = 1; one->b = 2; one->c = 3;
    two->a = 4; two->b = 5; two->c = 6;
    three->a = 7; three->b = 8; three->c = 9;

    list = g_list_append(list, one);
    list = g_list_append(list, two);
    list = g_list_append(list, three);

/*    list = g_list_first(list); */

    list = g_list_find_custom(list, "zwei", (GCompareFunc) (compare_name));
    
    x = list->data;
    printf("%d\n", (int) x->a);

    g_list_free(list);

    g_free(three);
    g_free(two);
    g_free(one);

    return 0;
}
