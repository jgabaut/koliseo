// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2023-2026  jgabaut

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

#include "./kls_banner.h"
/**
 * Defines title banner.
 */
char *kls_title[KLS_TITLEROWS + 1] = {
    "                               .',,,.                                                               ",
    "                        ...''',,;;:cl;.                                                             ",
    "                  ..''''''....      .co,                                                            ",
    "              .'',,,'..               'ol.                                                          ",
    "          ..''''..                     .co.                                                         ",
    "        ..'..                            cl.                                                        ",
    "      ..'...               .          .  .:'...       .       .      ..      .....     ...          ",
    "     ....      .      ..:c,'.   .oc .ll.  :dddxo.    ld.     'x:  .cxddx;   :kxodo,  .lddxx;        ",
    "    .,..     .cl,..   .cxd:..   ;Ol;do.  :Ol. c0c   .kx.     lO;  ;0d..::  .xk'     .xx' 'kx.       ",
    "    .,..     'dOl.     .''......lKOkc.  .kx.  ;0l   ;0l     .xk.  'xk;     'Ok;'.   l0;  .xk.       ",
    "    ''.;,    .,;;............. .xKKO'   ;0c   c0:   lO;     'Od.   .lko.   :0koo:. .xk.  'Od.       ",
    "   ',.'c;.  .......            'Oo:ko.  c0;  .xk.  .xx.     :0c  .   ;Od. .oO,     'Od.  c0:        ",
    "   ,;  .......        .::.     :O; lO;  :0l..okc.  'Od...  .oO' .ld'.:Od. 'kk,...  .kk,.:kd.        ",
    "   ;xc,...  .   'd:   .:d;     ;l. .l:  .:dool''c. 'ddolc. .cc.  'ldooc.  'dxoll:.  'odoo:.         ",
    "  .lx;. .  ,d;  .ll.   .''.                     ;l:'                                                ",
    "  :o.   ;,  cc    ..     .                       ,cc:::c,                                           ",
    " .c;    ..  .'           ....',,;;;;;;;;;;,,,,,,,,,,;;;ox;........'cddoollcc:;,,'..                 ",
    "  ,:         ...',:clodxkO0KKXXXXXXKKK000000KKKKXXXXKKKXXXKKKKKKKKKXNNNNNNNNNNNXXKOxoc;'.           ",
    "  .:. ..';cldkOKXXXXK0Okxxdolc::;,''.','......';clc::cloONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNK0xl;.       ",
    "  :xxxOKKK0kxoddl;,';od;.   'cl,    .ckko.    ,d00Ol.   .xXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNKkl;.   ",
    ".lKX0xoc,';;. .ll.   ;xO;   .oX0,    ,ONXl    ,0NNNK;    .lKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNX0o' ",
    ".xk;.  .  .;.  .:,    .dc    'OK;    .dNNo.   ;0NNNXc      ;kXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNk.",
    " :c        ..   ..    .l,    .k0,    .xXXo    ;0XNNXc       .l0NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNO'",
    " :;                   .'.    .;:.    .,cl;....,loddo;..       'dKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN0,",
    ".c,        ....',;;:cclllooddddddddddodxxxxxxxdddddddolllccccc:cxXNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNK;",
    ".l:',:cloxxkkkOkkxxdollcc::;,,'''.............................',cONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXc",
    ":00OOxdoc:;,'...                     ..        ..         .     .oNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
    ":o,..',. ..   ';.   .co'     ;d,    'oOk;    .o00kd;    .oOkd,   :KNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
    ":;   ,o'  ..  .l:    ;0o    .dXo.   'xNNk.   'ONNNNO'   :KNNNd.   cKNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNXo",
    "c;    ..  ..   ';    'ko.   'ONo.   .kNNO'   ,0NNNNK;   :KNNXd.    :0NNNNNNNNNNNNNNNNNNNNNNNNNNNNNXo",
    "l,             ..    'ko    'OXl    ,0NNO'   'ONNNNK;   ;KNNXl.     ;ONNNNNNNNNNNNNNNNNNNNNNNNNNNNXl",
    "cc.............';'...;do'...:k0c....:0XX0:...'xXXXX0c...:0XK0l.......c0NNNNNNNNNNNNNNNNNNNNXXXXXXKO,",
    ".;:;;;;;;:::::::::cc:::::::::c:;;;;,;ccc:;,,,,:cccc:;;;;;ccc:;,,,,,;;;clloooooooooooooooooollcc:;,. "
};

/**
 * Prints the title banner to the passed FILE pointer.
 * @see kls_title
 * @param fp The FILE to print to.
 */
void kls_print_title_2file(FILE *fp)
{
    if (fp == NULL) {
        fprintf(stderr,
                "[KLS] kls_print_title_2file():  Passed file pointer was NULL.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < KLS_TITLEROWS; i++) {
        fprintf(fp, "%s\n", kls_title[i]);
    }
}

/**
 * Prints the title banner to stdout.
 * @see kls_title
 */
void kls_print_title(void)
{
    kls_print_title_2file(stdout);
}
