#include <stdio.h>
#include <math.h>

int main()
{
    double field_width, field_height, field_area;
    double atom_width, atom_height, atom_area;
    int number, n, atoms_per_row, atoms_per_column, diff;

    field_width = 378;
    field_height = 266;
    number = 76;

    field_area = field_width * field_height;


    diff = -1;
    n = number;
    while (diff != 0) {
        atom_area = field_area / number;

        atom_width = sqrt(atom_area);
        atom_height = atom_area / atom_width;
        
        atoms_per_row = (int) (field_width / atom_width);
        atoms_per_column = (int) (number / atoms_per_row);

        diff = number - (atoms_per_row * atoms_per_column);
        printf("rows: %d, columns: %d, diff: %d, width: %f\n",
                atoms_per_row, atoms_per_column, diff, atom_width);
        field_area -= atom_area;
    }

    return 0;
}
