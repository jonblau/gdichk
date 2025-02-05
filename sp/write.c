# include <stdio.h>
# include <stdlib.h>
# include <string.h>

// Write
// Generates pregap and null data
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

int write_null (const char *path, const int size)
{
     FILE *file = fopen (path, "wb") ;
     if (file == NULL) { exit (EXIT_FAILURE) ; }

     unsigned char *null = calloc (size, 1) ;
     if (null == NULL) { exit (EXIT_FAILURE) ; }

     for (int i = 0 ; i < size ; i++) { null[i] = 0 ; }

     fwrite (null, 1, size, file) ;

     free (null) ;

     fclose (file) ;
     file = NULL ;

     return 0 ;
}

int write_sector (const char *path, int bcd_min, int bcd_sec, int bcd_fra)
{
     FILE *file = fopen (path, "ab") ;
     if (file == NULL) { exit (EXIT_FAILURE) ; }

     // sync

     unsigned char sync[12] = {0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0} ;
     fwrite (sync, 1, 12, file) ;

     // time

     unsigned char min = ((unsigned char) bcd_min / 10) * 16 + (bcd_min % 10) ;
     fwrite (&min, 1, 1, file) ;
     unsigned char sec = ((unsigned char) bcd_sec / 10) * 16 + (bcd_sec % 10) ;
     fwrite (&sec, 1, 1, file) ;
     unsigned char fra = ((unsigned char) bcd_fra / 10) * 16 + (bcd_fra % 10) ;
     fwrite (&fra, 1, 1, file) ;

     // mode

     unsigned char mode = 1 ;
     fwrite (&mode, 1, 1, file) ;

     // data

     unsigned char data[2336] = {0} ;
     fwrite (data, 1, 2336, file) ;

     fclose (file) ;
     file = NULL ;

     return 0 ;
}

int write_pregap (const char *path, int bcd_min, int bcd_sec, int bcd_fra)
{
     // null pregap

     write_null (path, 176400) ;

     // data pregap

     for (int i = 0 ; i < 150 ; i++)
     {
          write_sector (path, bcd_min, bcd_sec, bcd_fra) ;

          bcd_fra += 1 ;

          if (bcd_fra > 74) { bcd_fra = 0 ; bcd_sec += 1 ; }
          if (bcd_sec > 59) { bcd_sec = 0 ; bcd_min += 1 ; }
     }

     return 0 ;
}

int show_usage (void)
{
     fprintf (stderr, "write [mode] [path] (optional) [mm] [ss] [ff] [size]\n\n"

                      "mode          -a : audio track pregap\n"
                      "                   requires path\n"
                      "              -d : data track pregap\n"
                      "                   requires path and timing\n"
                      "              -n : null data\n"
                      "                   requires path and size\n") ;

     return 0 ;
}

int main (int argc, char **argv)
{
	if (argc < 3) { show_usage () ; exit (1) ; }

     char *mode = argv[1] ;
     char *path = argv[2] ;

     if (strcmp (mode, "-a") == 0 && argc == 3)
     {
          write_null (path, 352800) ;
     }

     else if (strcmp (mode, "-d") == 0 && argc == 6)
     {
          int bcd_min = atoi (argv[3]) ;
          int bcd_sec = atoi (argv[4]) ;
          int bcd_fra = atoi (argv[5]) ;

          write_pregap (path, bcd_min, bcd_sec, bcd_fra) ;
     }

     else if (strcmp (mode, "-n") == 0 && argc == 4)
     {
          int size = atoi (argv[3]) ;

          write_null (path, size) ;
     }

     else
     {
          show_usage () ; exit (1) ;
     }

     return 0 ;
}
