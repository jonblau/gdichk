# include <stdio.h>
# include <stdlib.h>

// Read
// Prints any file in hexadecimal
//
// Copyright (c) 2025 Jon Blau

// SPDX-License-Identifier: GPL-3.0-or-later
//
// This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
//  along with this program. If not, see <https://www.gnu.org/licenses/>.

int read_hex (const char *path, const int size, const int skip)
{
     FILE *file = fopen (path, "rb") ;
     if (file == NULL) { exit (EXIT_FAILURE) ; }

     fseek (file, 0, SEEK_END) ;
     long file_size = ftell (file) ;

     if (size + skip > file_size)
     {
          fprintf (stderr, "not enough data to read\n") ;
          exit (2) ;
     }

     fseek (file, skip, SEEK_SET) ;
     unsigned char byte = 0 ;

     for (int i = 0 ; i < size ; i++)
     {
          fread (&byte, 1, 1, file) ;

          if (byte < 16) { printf ("0%x ", byte) ; }
          else { printf ("%x ", byte) ; }
     }

     printf ("\n") ;

     fclose (file) ;
     file = NULL ;

     return 0 ;
}

int show_usage (void)
{
     fprintf (stderr, "read [path] [size] [skip]\n") ;

     return 0 ;
}

int main (int argc, char **argv)
{
	if (argc != 4) { show_usage () ; exit (1) ; }

     char *path = argv[1] ;

     int size = atoi (argv[2]) ;
     int skip = atoi (argv[3]) ;

     if (size <= 0 || skip < 0) { show_usage () ; exit (1) ; }

     read_hex (path, size, skip) ;

     return 0 ;
}
