#include <stdio.h>

typedef enum
{
    G_SIGNAL_MATCH_ID        = 1 << 0,
    G_SIGNAL_MATCH_DETAIL    = 1 << 1,
    G_SIGNAL_MATCH_CLOSURE   = 1 << 2,
    G_SIGNAL_MATCH_FUNC      = 1 << 3,
    G_SIGNAL_MATCH_DATA      = 1 << 4,
    G_SIGNAL_MATCH_UNBLOCKED = 1 << 5
} GSignalMatchType;

int main()
{
    GSignalMatchType mask;

    mask = 0 << 0;

    if (mask & (G_SIGNAL_MATCH_CLOSURE | G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA))
        printf("JA\n");
    else
        printf("NEIN\n");

    return 0;
}
