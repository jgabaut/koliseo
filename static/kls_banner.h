// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2023-2024  jgabaut

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef KLS_TITLE_H_
#define KLS_TITLE_H_
#include <stdio.h>
#include <stdlib.h>

#define KLS_TITLEROWS 33 /**< Defines how many rows the title banner has.*/
extern char *kls_title[KLS_TITLEROWS + 1];
/**< Contains title banner.*/

void kls_print_title_2file(FILE * fp);/**< Prints the title banner to the passed FILE.*/
void kls_print_title(void);
#endif // KLS_TITLE_H_
